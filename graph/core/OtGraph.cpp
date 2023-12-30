//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <fstream>
#include <sstream>
#include <random>
#include <set>

#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtGraph.h"
#include "OtGraphPin.h"
#include "OtGraphUtils.h"


//
//	OtGraph::clear
//

void OtGraph::clear() {
	nodes.clear();
	links.clear();
	nodeIndex.clear();
	pinIndex.clear();
}


//
//	OtGraph::load
//

void OtGraph::load(const std::filesystem::path& path, nlohmann::json* metadata) {
	// load scene from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.string().c_str());

		if (stream.fail()) {
			OtError("Can't read from file [%s]", path.string().c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [%s], error: %s", path.string().c_str(), e.what());
	}

	// parse json
	auto data = nlohmann::json::parse(buffer.str());

	// clear graph
	clear();

	// extract metadata
	if (metadata) {
		*metadata = data["metadata"];
	}

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
//	OtGraph::save
//

void OtGraph::save(const std::filesystem::path& path, nlohmann::json* metadata) {
	// create cjosn outline
	auto data = nlohmann::json::object();
	data["metadata"] = metadata ? *metadata : nlohmann::json::object();
	auto nodes = nlohmann::json::array();
	auto links = nlohmann::json::array();

	// save all nodes
	eachNode([&](OtGraphNode& node) {
		nodes.push_back(node->serialize());
	});

	// save all links
	eachLink([&](OtGraphLink& link) {
		links.push_back(link->serialize());
	});

	data["nodes"] = nodes;
	data["links"] = links;

	// write graph to file
	try {
		std::ofstream stream(path.string().c_str());

		if (stream.fail()) {
			OtError("Can't open file [%s] for writing", path.string().c_str());
		}

		stream << data.dump(1, '\t');
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't write to file [%s], error: %s", path.string().c_str(), e.what());
	}
}


//
//	OtGraph::createNode
//

OtGraphNode OtGraph::createNode(const std::string& name, float x, float y) {
	// create a new node
	auto node = factory.createNode(name);
	nodes.emplace_back(node);
	node->x = x;
	node->y = y;
	node->needsPlacement = true;

	// index node and pins
	indexNode(node);

	// return new mode
	return node;
}


//
//	OtGraph::deleteNode
//

void OtGraph::deleteNode(OtGraphNode node) {
	// find the node
	auto i = std::find_if(nodes.begin(), nodes.end(), [node] (OtGraphNode& candidate) {
		return candidate->id == node->id;
	});

	// remove registered pins and possible links
	(*i)->eachInput([this](OtGraphPin& pin) {
		deleteLinks(pin);
		pinIndex.erase(pin->id);
	});

	(*i)->eachOutput([this](OtGraphPin& pin) {
		deleteLinks(pin);
		pinIndex.erase(pin->id);
	});

	// remove specified node
	nodeIndex.erase(node->id);
	nodes.erase(i);
}


//
//	OtGraph::deleteNodes
//

void OtGraph::deleteNodes(const std::vector<uint32_t>& nodes) {
	for (auto id : nodes) {
		deleteNode(id);
	}
}


//
//	OtGraph::isLinkValid
//

bool OtGraph::isLinkValid(OtGraphPin from, OtGraphPin to) {
	if (from->isInput() || to->isOutput()) {
		return false;

	} else if (to->isConnected()) {
		return false;

	} else if (from->type != to->type) {
		return false;
	}

	return true;
}


//
//	OtGraph::createLink
//

OtGraphLink OtGraph::createLink(OtGraphPin from, OtGraphPin to, uint32_t id) {
	// add new link
	auto link = std::make_shared<OtGraphLinkClass>(from, to, id);
	links.emplace_back(link);
	linkIndex[link->id] = link;
	link->connect();

	// re-sort node list
	sortNodes();
	return link;
}


//
//	OtGraph::findLink
//

OtGraphLink OtGraph::findLink(OtGraphPin from, OtGraphPin to) {
	// find the link
	auto i = std::find_if(links.begin(), links.end(), [&] (OtGraphLink& candidate) {
		return candidate->from == from && candidate->to == to;
	});

	return *i;
}


//
//	OtGraph::deleteLink
//

void OtGraph::deleteLink(OtGraphLink link) {
	// remove specified link
	link->disconnect();
	linkIndex.erase(link->id);

	links.erase(std::remove_if(links.begin(), links.end(), [link] (OtGraphLink& candidate) {
		return candidate == link;
	}), links.end());

	// re-sort node list
	sortNodes();
}

void OtGraph::deleteLink(OtGraphPin from, OtGraphPin to) {
	// find the link
	auto i = std::find_if(links.begin(), links.end(), [&] (OtGraphLink& candidate) {
		return candidate->from == from && candidate->to == to;
	});

	deleteLink(*i);
}


//
//	OtGraph::deleteLinks
//

void OtGraph::deleteLinks(OtGraphPin any) {
	// find links
	std::vector<OtGraphLink> associatedLinks;

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
//	OtGraph::redirectLink
//

void OtGraph::redirectLink(OtGraphLink link, uint32_t newTo) {
	link->disconnect();
	link->redirectTo(pinIndex[newTo]);
	link->connect();
}


//
//	OtGraph::selectAll
///

void OtGraph::selectAll() {
	eachNode([](OtGraphNode& node) {
		node->selected = true;
	});
}


//
//	OtGraph::deselectAll
//

void OtGraph::deselectAll() {
	eachNode([] (OtGraphNode& node) {
		node->selected = false;
	});
}


//
//	OtGraph::select
//

void OtGraph::select(uint32_t id, bool deselect) {
	if (deselect) {
		deselectAll();
	}

	nodeIndex[id]->selected = true;
}

void OtGraph::select(const std::vector<uint32_t>& nodes, bool deselect) {
	if (deselect) {
		deselectAll();
	}

	for (auto id : nodes) {
		nodeIndex[id]->selected = true;
	}
}

void OtGraph::select(int x1, int y1, int x2, int y2) {
	deselectAll();

	eachNode([&] (OtGraphNode& node) {
		node->selected =
			x1 < (node->x + node->w) && x2 > node->x &&
			y1 < (node->y + node->h) && y2 > node->y;
	});
}


//
//	OtGraph::getSelected
//

std::vector<uint32_t> OtGraph::getSelected() {
	std::vector<uint32_t> result;

	for (auto node : nodes) {
		if (node->selected) {
			result.push_back(node->id);
		}
	}

	return result;
}


//
//	OtGraph::archiveNode
//

std::string OtGraph::archiveNode(uint32_t node) {
	std::vector<uint32_t> selection = {node};
	return archiveNodes(selection);
}


//
//	OtGraph::archiveNodes
//

std::string OtGraph::archiveNodes(const std::vector<uint32_t>& selection) {
	auto nodes = nlohmann::json::array();
	auto links = nlohmann::json::array();
	std::set<OtGraphLink> associatedLinks;

	for (auto id : selection) {
		auto node = nodeIndex[id];
		nodes.push_back(node->serialize());

		eachLink([&](OtGraphLink& link) {
			if (link->from->node == node || link->to->node == node) {
				associatedLinks.insert(link);
			}
		});
	}

	for (auto& link : associatedLinks) {
		links.push_back(link->serialize());
	}

	auto data = nlohmann::json::object();
	data["nodes"] = nodes;
	data["links"] = links;
	return data.dump();
}


//
//	OtGraph::restoreNodes
//

void OtGraph::restoreNodes(const std::string& json) {
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
//	OtGraph::duplicateNodes
//

std::vector<uint32_t> OtGraph::duplicateNodes(const std::string& json) {
	auto data = nlohmann::json::parse(json);
	std::vector<uint32_t> nodes;

	// restore each node
	for (auto& node : data["nodes"]) {
		auto newNode = restoreNode(node, false);
		newNode->x += 50.0f;
		newNode->y += 50.0f;
		nodes.push_back(newNode->id);
	}

	// create list of node IDs
	return nodes;
}


//
//	OtGraph::indexNode
//

void OtGraph::indexNode(OtGraphNode node) {
	// index node and its pins
	nodeIndex[node->id] = node;

	node->eachInput([this](OtGraphPin& pin) {
		if (!pin) {
			int i = 0;
		}

		pinIndex[pin->id] = pin;
	});

	node->eachOutput([this](OtGraphPin& pin) {
		if (!pin) {
			int i = 0;
		}

		pinIndex[pin->id] = pin;
	});
}


//
//	OtGraph::unindexNode
//

void OtGraph::unindexNode(OtGraphNode node) {
	nodeIndex.erase(nodeIndex.find(node->id));

	node->eachInput([this](OtGraphPin& pin) {
		pinIndex.erase(pinIndex.find(pin->id));
	});

	node->eachOutput([this](OtGraphPin& pin) {
		pinIndex.erase(pinIndex.find(pin->id));
	});
}


//
//	OtGraph::restoreNode
//

OtGraphNode OtGraph::restoreNode(nlohmann::json data, bool restoreIDs) {
	// create a new node
	auto node = factory.createNode(data["name"]);
	node->deserialize(data, restoreIDs);
	node->needsPlacement = true;
	nodes.emplace_back(node);

	// index node and pins
	indexNode(node);

	// return new node
	return node;
}


//
//	OtGraph::sortNodes
//

void OtGraph::sortNodes() {
	// TODO
}
