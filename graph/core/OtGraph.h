//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtGraphNode.h"
#include "OtGraphLink.h"
#include "OtGraphPin.h"



//
//	OtGraph
//

class OtGraph {
public:
	// register a node type
	template <typename T>
	void registerNodeType(const char* name) {
		nodeTypes[name] = []() {
			auto node = std::make_shared<T>();
			node->configure();
			return node;
		};
	}

	// clear the entire graph
	void clear();

	// load and save scene
	void load(const std::filesystem::path& path, nlohmann::json* metadata=nullptr);
	void save(const std::filesystem::path& path, nlohmann::json* metadata=nullptr);

	// manipulate nodes
	OtGraphNode createNode(const std::string& name, float x, float y);
	void deleteNode(int id) { deleteNode(nodeIndex[id]); }
	void deleteNode(OtGraphNode node);

	// manipulate links
	bool isLinkValid(int from, int to) { return isLinkValid(pinIndex[from], pinIndex[to]); }
	bool isLinkValid(OtGraphPin from, OtGraphPin to);

	int createLink(int from, int to, int id=0) { return createLink(pinIndex[from], pinIndex[to]); }
	int createLink(OtGraphPin from, OtGraphPin to, int id=0);

	void deleteLink(int from, int to) { deleteLink(pinIndex[from], pinIndex[to]); }
	void deleteLink(OtGraphPin from, OtGraphPin to);
	void deleteLink(int any) { deleteLink(pinIndex[any]); }
	void deleteLink(OtGraphPin any);

	// access nodes and pins
	OtGraphNode& getNode(int id) { return nodeIndex[id]; }
	OtGraphPin& getPin(int id) { return pinIndex[id]; }

	// iterate through nodes and links
	void eachNode(std::function<void(OtGraphNode&)> callback) {
		for (auto& node : nodes) {
			callback(node);
		}
	}

	void eachLink(std::function<void(OtGraphLink&)> callback) {
		for (auto& link : links) {
			callback(link);
		}
	}

	// archive/restore nodes to/from a string (support for do/undo/redo operations)
	std::string archiveNodes(const std::vector<int>& nodes);
	void deleteNodes(const std::vector<int>& nodes);
	void restoreNodes(const std::string& json);
	void duplicateNodes(const std::string& json);

	// (re)evaluate entire graph
	void evaluate();

private:
	// properties
	std::unordered_map<std::string, std::function<OtGraphNode()>> nodeTypes;

	std::vector<OtGraphNode> nodes;
	std::vector<OtGraphLink> links;

	std::unordered_map<int, OtGraphNode> nodeIndex;
	std::unordered_map<int, OtGraphPin> pinIndex;

	// (un)index a node and its pins
	void indexNode(OtGraphNode node);
	void unindexNode(OtGraphNode node);

	// restore a node from its JSON data
	int restoreNode(nlohmann::json data, bool restoreID=true);

	// topographically sort the nodes
	void sortNodes();
};
