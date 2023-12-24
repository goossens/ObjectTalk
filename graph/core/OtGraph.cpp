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
	auto nodes = data["nodes"] = nlohmann::json::array();
	auto links = data["links"] = nlohmann::json::array();

	// save all nodes
	eachNode([&](OtGraphNode& node) {
		nodes.push_back(node->serialize());
	});

	// save all links
	eachLink([&](OtGraphLink& link) {
		links.push_back(link.serialize());
	});

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
	auto node = nodeTypes[name]();
	nodes.emplace_back(node);
	node->x = x;
	node->y = y;

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

	// remove registered pins
	(*i)->eachInput([this](OtGraphPin& pin) {
		deleteLink(pin->id);
		pinIndex.erase(pin->id);
	});

	(*i)->eachOutput([this](OtGraphPin& pin) {
		deleteLink(pin->id);
		pinIndex.erase(pin->id);
	});

	// remove specified node
	nodeIndex.erase(node->id);
	nodes.erase(i);
}


//
//	OtGraph::isLinkValid
//

bool OtGraph::isLinkValid(OtGraphPin from, OtGraphPin to) {
	if (from == to) {
		return false;

	} else if (from->isInput() || to->isOutput()) {
		return false;
	}

	return true;
}


//
//	OtGraph::createLink
//

int OtGraph::createLink(OtGraphPin from, OtGraphPin to, int id) {
	// add new link
	links.emplace_back(OtGraphLink(from, to, id));
	to->setSource(from);

	// re-sort node list
	sortNodes();
	return links.back().id;
}

//
//	OtGraph::deleteLink
//

void OtGraph::deleteLink(OtGraphPin from, OtGraphPin to) {
	// remove specified link
	links.erase(std::remove_if(links.begin(), links.end(), [from, to] (OtGraphLink& candidate) {
		return candidate.from->id == from->id && candidate.to->id == to->id;
	}), links.end());

	// re-sort node list
	sortNodes();
}

void OtGraph::deleteLink(OtGraphPin any) {
	// remove specified link
	links.erase(std::remove_if(links.begin(), links.end(), [any] (OtGraphLink& candidate) {
		return candidate.from->id == any->id || candidate.to->id == any->id;
	}), links.end());

	// re-sort node list
	sortNodes();
}


//
//	OtGraph::archiveNodes
//

std::string OtGraph::archiveNodes(const std::vector<int>& selection) {
	auto data = nlohmann::json::object();
	auto nodes = data["nodes"] = nlohmann::json::array();
	auto links = data["links"] = nlohmann::json::array();

	std::set<OtGraphLink*> associatedLinks;

	for (auto id : selection) {
		auto node = nodeIndex[id];
		nodes.push_back(node->serialize());

		eachLink([&](OtGraphLink& link) {
			if (link.from->node == node || link.to->node == node) {
				associatedLinks.insert(&link);
			}
		});
	}

	for (auto& link : associatedLinks) {
		links.push_back(link->serialize());
	}

	return data.dump();
}


//
//	OtGraph::deleteNodes
//

void OtGraph::deleteNodes(const std::vector<int>& nodes) {
	for (auto id : nodes) {
		deleteNode(id);
	}
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

void OtGraph::duplicateNodes(const std::string& json) {
	auto data = nlohmann::json::parse(json);

	// restore each node
	for (auto& node : data["nodes"]) {
		restoreNode(node, false);
	}
}


//
//	OtGraph::indexNode
//

void OtGraph::indexNode(OtGraphNode node) {
	// index node and its pins
	nodeIndex[node->id] = node;

	node->eachInput([this](OtGraphPin& pin) {
		pinIndex[pin->id] = pin;
	});

	node->eachOutput([this](OtGraphPin& pin) {
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

int OtGraph::restoreNode(nlohmann::json data, bool restoreID) {
	// create a new node
	auto node = nodeTypes[data["name"]]();
	node->deserialize(data, restoreID);
	nodes.emplace_back(node);

	// index node and pins
	indexNode(node);

	// return ID
	return node->id;
}


//
//	OtGraph::sortNodes
//

void OtGraph::sortNodes() {
	// TODO
}
