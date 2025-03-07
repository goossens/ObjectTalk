//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <set>

#include "nlohmann/json.hpp"

#include "OtLog.h"
#include "OtNodes.h"
#include "OtNodesPin.h"
#include "OtNodesUtils.h"
#include "OtPath.h"
#include "OtText.h"


//
//	OtNodes::~OtNodes
//

OtNodes::~OtNodes() {
	// ensure we dispose in the right order due to circular dependencies
	clear();
}


//
//	OtNodes::clear
//

void OtNodes::clear() {
	linkIndex.clear();
	pinIndex.clear();
	nodeIndex.clear();
	links.clear();
	nodes.clear();
	metadata = "{}";
}


//
//	OtNodes::load
//

void OtNodes::load(const std::string& path) {
	// load scene from file
	auto basedir = OtPath::getParent(path);
	loadFromString(OtText::load(path), basedir);
}


//
//	OtNodes::loadFromString
//

void OtNodes::loadFromString(const std::string& string, std::string& basedir) {
	// clear nodes
	clear();

	// treat an empty file as a blank nodes
	if (string.size()) {
		// parse json
		auto data = nlohmann::json::parse(string);

		// extract metadata
		if (data.contains("metadata")) {
			metadata = data["metadata"].dump();
		}

		// restore each node
		for (auto& node : data["nodes"]) {
			restoreNode(node, true, &basedir);
		}

		// restore links
		for (auto& link : data["links"]) {
			uint32_t linkId = link["id"];
			uint32_t fromId = link["from"];
			uint32_t toId = link["to"];

			if (pinIndex.count(fromId) == 0) {
				OtLogError("Invalid 'from' pin ID [{}] in link [{}]", fromId, linkId);

			} else if (pinIndex.count(toId) == 0) {
				OtLogError("Invalid 'to' pin ID [{}] in link [{}]", toId, linkId);
			}

			createLink(fromId, toId, linkId);
		}
	}

	// set the sorting flag
	needsSorting = true;
}


//
//	OtNodes::save
//

void OtNodes::save(const std::string& path) {
	// create json outline
	auto data = nlohmann::json::object();
	data["metadata"] = nlohmann::json::parse(metadata.c_str());
	auto nodesJSON = nlohmann::json::array();
	auto linksJSON = nlohmann::json::array();

	// save all nodes
	auto basedir = OtPath::getParent(path);

	eachNode([&](OtNode& node) {
		nodesJSON.push_back(node->serialize(&basedir));
	});

	// save all links
	eachLink([&](OtNodesLink& link) {
		linksJSON.push_back(link->serialize(&basedir));
	});

	data["nodes"] = nodesJSON;
	data["links"] = linksJSON;

	// write nodes to file
	OtText::save(path, data.dump(1, '\t'));
}


//
//	OtNodes::createNode
//

OtNode OtNodes::createNode(const std::string& name, float x, float y) {
	// create a new node
	auto node = OtNodesFactory::createNode(name);
	nodes.emplace_back(node);
	needsSorting = true;

	node->x = x;
	node->y = y;
	node->needsPlacement = true;
	node->needsSizing = true;
	node->needsEvaluating = true;

	// index node and pins
	indexNode(node);

	// return new mode
	return node;
}


//
//	OtNodes::deleteNode
//

void OtNodes::deleteNode(OtNode node) {
	// find the node
	auto i = std::find_if(nodes.begin(), nodes.end(), [node](OtNode& candidate) {
		return candidate->id == node->id;
	});

	// remove registered pins and possible links
	(*i)->eachInput([this](OtNodesPin& pin) {
		deleteLinks(pin);
		pinIndex.erase(pin->id);
	});

	(*i)->eachOutput([this](OtNodesPin& pin) {
		deleteLinks(pin);
		pinIndex.erase(pin->id);
	});

	// remove specified node
	nodeIndex.erase(node->id);
	nodes.erase(i);
	needsSorting = true;
}


//
//	OtNodes::deleteNodes
//

void OtNodes::deleteNodes(const std::vector<uint32_t>& nodeIDs) {
	for (auto id : nodeIDs) {
		deleteNode(id);
	}
}


//
//	OtNodes::hasCycle
//

bool OtNodes::hasCycle(OtNodeClass* node, OtNodeClass* newTarget) {
	// function result
	bool cycle = false;

	// only process each node once
	if (!node->permanentMark) {
		// check for cycles
		if (node->temporaryMark) {
			cycle = true;

		} else {
			// temporarily mark the node so we can detect cycles
			node->temporaryMark = true;

			// visit all nodes it depends on
			node->eachInput([&](OtNodesPin& pin) {
				if (!cycle && pin->sourcePin != nullptr) {
					cycle = hasCycle(pin->sourcePin->node);
				}
			});

			// also check the possible new connection (if required)
			if (!cycle && newTarget) {
				cycle = hasCycle(newTarget);
			}

			// set proper flags
			node->temporaryMark = false;
			node->permanentMark = true;
		}
	}

	return cycle;
}


//
//	OtNodes::causesCycle
//

bool OtNodes::causesCycle(OtNodesPin from, OtNodesPin to) {
	// based on https://en.wikipedia.org/wiki/Topological_sorting

	// clear all flags
	for (auto& node : nodes) {
		node->permanentMark = false;
		node->temporaryMark = false;
	}

	// visit the nodes in the proposed link (depth first)
	if (hasCycle(to->node, from->node)) {
		// we have a cycle
		return true;
	}

	// no cycle detected
	return false;
}


//
//	OtNodes::isLinkValid
//

bool OtNodes::isLinkValid(OtNodesPin from, OtNodesPin to) {
	if (from->isInput() || to->isOutput()) {
		return false;

	} else if (to->isSourceConnected()) {
		return false;

	} else if (from->type != to->type) {
		return false;

	} else if (causesCycle(from, to)) {
		return false;
	}

	return true;
}


//
//	OtNodes::createLink
//

OtNodesLink OtNodes::createLink(OtNodesPin from, OtNodesPin to, uint32_t id) {
	// add new link
	auto link = std::make_shared<OtNodesLinkClass>(from, to, id);
	links.emplace_back(link);
	linkIndex[link->id] = link;
	link->connect();

	// set the flags
	to->node->needsEvaluating = true;
	to->node->needsSizing = true;
	needsSorting = true;

	// return new link
	return link;
}


//
//	OtNodes::findLink
//

OtNodesLink OtNodes::findLink(OtNodesPin from, OtNodesPin to) {
	// find the link
	auto i = std::find_if(links.begin(), links.end(), [&](OtNodesLink& candidate) {
		return candidate->from == from && candidate->to == to;
	});

	return *i;
}


//
//	OtNodes::deleteLink
//

void OtNodes::deleteLink(OtNodesLink link) {
	// ensure node size is recalculated
	link->to->node->needsEvaluating = true;
	link->to->node->needsSizing = true;

	// remove specified link
	link->disconnect();
	linkIndex.erase(link->id);

	links.erase(std::remove_if(links.begin(), links.end(), [link](OtNodesLink& candidate) {
		return candidate == link;
	}), links.end());

	// set sorting flag
	needsSorting = true;
}

void OtNodes::deleteLink(OtNodesPin from, OtNodesPin to) {
	// find the link
	auto i = std::find_if(links.begin(), links.end(), [&](OtNodesLink& candidate) {
		return candidate->from == from && candidate->to == to;
	});

	// ensure node size is recalculated
	to->node->needsSizing = true;

	deleteLink(*i);
}


//
//	OtNodes::deleteLinks
//

void OtNodes::deleteLinks(OtNodesPin any) {
	// find links
	std::vector<OtNodesLink> associatedLinks;

	for (auto& link : links) {
		if (link->from == any || link->to == any) {
			associatedLinks.push_back(link);
		}
	}

	// remove discovered links
	for (auto& link : associatedLinks) {
		deleteLink(link);
	}
}


//
//	OtNodes::redirectLink
//

void OtNodes::redirectLink(OtNodesLink link, uint32_t newTo) {
	// redirect link
	link->to->node->needsEvaluating = true;
	link->to->node->needsSizing = true;
	link->disconnect();
	link->redirectTo(pinIndex[newTo]);
	link->connect();

	// set flags
	link->to->node->needsEvaluating = true;
	link->to->node->needsSizing = true;
	needsSorting = true;
}


//
//	OtNodes::selectAll
///

void OtNodes::selectAll() {
	for (auto& node : nodes) {
		node->selected = true;
	};
}


//
//	OtNodes::deselectAll
//

void OtNodes::deselectAll() {
	for (auto& node : nodes) {
		node->selected = false;
	};
}


//
//	OtNodes::select
//

void OtNodes::select(uint32_t id, bool deselect) {
	if (deselect) {
		deselectAll();
	}

	nodeIndex[id]->selected = true;
}

void OtNodes::select(const std::vector<uint32_t>& nodeIDs, bool deselect) {
	if (deselect) {
		deselectAll();
	}

	for (auto id : nodeIDs) {
		nodeIndex[id]->selected = true;
	}
}

void OtNodes::select(int x1, int y1, int x2, int y2) {
	deselectAll();

	for (auto& node : nodes) {
		node->selected =
			x1 < (node->x + node->w) && x2 > node->x &&
			y1 < (node->y + node->h) && y2 > node->y;
	};
}


//
//	OtNodes::deselect
//

void OtNodes::deselect(uint32_t id) {
	nodeIndex[id]->selected = false;
}


//
//	OtNodes::getSelected
//

std::vector<uint32_t> OtNodes::getSelected() {
	std::vector<uint32_t> result;

	for (auto& node : nodes) {
		if (node->selected) {
			result.push_back(node->id);
		}
	}

	return result;
}


//
//	OtNodes::archiveNode
//

std::string OtNodes::archiveNode(uint32_t node) {
	std::vector<uint32_t> selection = {node};
	return archiveNodes(selection);
}


//
//	OtNodes::archiveNodes
//

std::string OtNodes::archiveNodes(const std::vector<uint32_t>& selection) {
	auto nodesJSON = nlohmann::json::array();
	auto linksJSON = nlohmann::json::array();
	std::set<OtNodesLink> associatedLinks;

	for (auto id : selection) {
		auto node = nodeIndex[id];
		nodesJSON.push_back(node->serialize());

		eachLink([&](OtNodesLink& link) {
			if (link->from->node->id == id || link->to->node->id == id) {
				associatedLinks.insert(link);
			}
		});
	}

	for (auto& link : associatedLinks) {
		linksJSON.push_back(link->serialize());
	}

	auto data = nlohmann::json::object();
	data["nodes"] = nodesJSON;
	data["links"] = linksJSON;
	return data.dump();
}


//
//	OtNodes::restoreNode
//

OtNode OtNodes::restoreNode(nlohmann::json data, bool restoreIDs, std::string* basedir) {
	// create a new node
	auto node = OtNodesFactory::createNode(data["type"]);
	node->deserialize(data, restoreIDs, basedir);
	nodes.emplace_back(node);
	needsSorting = true;

	node->needsPlacement = true;
	node->needsSizing = true;
	node->needsEvaluating = true;

	// index node and pins
	indexNode(node);

	// return new node
	return node;
}


//
//	OtNodes::restoreNodes
//

void OtNodes::restoreNodes(const std::string& json) {
	auto data = nlohmann::json::parse(json);

	// restore each node
	for (auto& node : data["nodes"]) {
		restoreNode(node);
	}

	// restore links
	for (auto& link : data["links"]) {
		createLink(link["from"], link["to"], link["id"]);
	}
}


//
//	OtNodes::duplicateNodes
//

std::vector<uint32_t> OtNodes::duplicateNodes(const std::string& json) {
	auto data = nlohmann::json::parse(json);
	std::vector<uint32_t> nodeIDs;

	// restore each node
	for (auto& node : data["nodes"]) {
		auto newNode = restoreNode(node, false);
		newNode->x += 50.0f;
		newNode->y += 50.0f;
		nodeIDs.push_back(newNode->id);
	}

	// create list of node IDs
	return nodeIDs;
}


//
//	OtNodes::indexNode
//

void OtNodes::indexNode(OtNode node) {
	// index node and its pins
	nodeIndex[node->id] = node;

	node->eachInput([this](OtNodesPin& pin) {
		pinIndex[pin->id] = pin;
	});

	node->eachOutput([this](OtNodesPin& pin) {
		pinIndex[pin->id] = pin;
	});
}


//
//	OtNodes::unindexNode
//

void OtNodes::unindexNode(OtNode node) {
	nodeIndex.erase(nodeIndex.find(node->id));

	node->eachInput([this](OtNodesPin& pin) {
		pinIndex.erase(pinIndex.find(pin->id));
	});

	node->eachOutput([this](OtNodesPin& pin) {
		pinIndex.erase(pinIndex.find(pin->id));
	});
}


//
//	OtNodes::visitNode
//

bool OtNodes::visitNode(OtNode& node, std::vector<OtNode>& nodeList) {
	// function result
	bool cycle = false;

	// only process each node once
	if (!node->permanentMark) {
		// check for cycles
		if (node->temporaryMark) {
			cycle = true;

		} else {
			// temporarily mark the node so we can detect cycles
			node->temporaryMark = true;

			// visit all nodes it depends on
			node->eachInput([&](OtNodesPin& pin) {
				if (!cycle && pin->sourcePin != nullptr) {
					cycle = visitNode(nodeIndex[pin->sourcePin->node->id], nodeList);
				}
			});

			// set proper flags and add node to list (if required)
			if (!cycle) {
				node->temporaryMark = false;
				node->permanentMark = true;
				nodeList.push_back(node);
			}
		}
	}

	return cycle;
}


//
//	OtNodes::sortNodesTopologically
//

bool OtNodes::sortNodesTopologically() {
	// based on https://en.wikipedia.org/wiki/Topological_sorting

	// clear all flags
	for (auto& node : nodes) {
		node->permanentMark = false;
		node->temporaryMark = false;
	}

	// place to store sorted nodes
	std::vector<OtNode> sortedNodes;

	// process all nodes
	for (auto& node : nodes) {
		if (visitNode(node, sortedNodes)) {
			return false;
		}
	}

	// sort was succesful
	nodes.swap(sortedNodes);
	return true;
}


//
//	OtNodes::classifyLinks
//

void OtNodes::classifyLinks() {
	for (auto& node : nodes) {
		node->varyingInput = false;

		node->eachInput([&](OtNodesPin& pin) {
			pin->varying = pin->sourcePin ? pin->sourcePin->varying : false;
			node->varyingInput |= pin->varying;
		});

		if (node->kind == OtNodeClass::Kind::fixed) {
			node->eachOutput([](OtNodesPin& pin) {
				pin->varying = false;
			});

		} else if (node->kind == OtNodeClass::Kind::varying) {
			node->eachOutput([](OtNodesPin& pin) {
				pin->varying = true;
			});

		} else {
			node->eachOutput([&](OtNodesPin& pin) {
				pin->varying = node->varyingInput;
			});
		}
	}
}


//
//	OtNodes::evaluate
//

void OtNodes::evaluate() {
	// see if resorting is required
	if (needsSorting) {
		sortNodesTopologically();
		classifyLinks();
		needsSorting = false;
		needsEvaluating = true;
	}

	// update all the nodes and see if we have to (re)run all nodes
	for (auto& node : nodes) {
		needsEvaluating |= node->onUpdate();
		needsEvaluating |= node->needsEvaluating;
	}

	// evaluate all nodes
	for (auto& node : nodes) {
		node->eachInput([node](OtNodesPin& pin) {
			if (pin->isVarying() && pin->sourcePin->node->needsEvaluating) {
				node->needsEvaluating = true;
			}

			node->needsEvaluating |= pin->processInput();
		});

		if (node->needsEvaluating) {
			node->onExecute();
		};
	}

	// reset all nodes
	for (auto& node : nodes) {
		node->needsEvaluating = false;
	}
}
