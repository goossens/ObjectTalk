//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtGraphNode.h"
#include "OtGraphNodeFactory.h"
#include "OtGraphLink.h"
#include "OtGraphPin.h"


//
//	OtGraph
//

class OtGraph {
public:
	// destructor
	~OtGraph();

	// register a node type
	template <typename T>
	inline void registerNodeType(const char* category, const char* name) {
		factory.registerType(
			category,
			name,
			[]() {
				auto node = std::make_shared<T>();
				node->configure();
				return node;
			});
	}

	// get node factory
	inline OtGraphNodeFactory& getNodeFactory() { return factory; }

	// clear the entire graph
	void clear();

	// load and save graph
	void load(const std::string& path);
	void save(const std::string& path);

	// manipulate nodes
	OtGraphNode createNode(const std::string& name, float x, float y);
	inline void deleteNode(uint32_t id) { deleteNode(nodeIndex[id]); }
	void deleteNode(OtGraphNode node);
	void deleteNodes(const std::vector<uint32_t>& nodes);
	inline size_t getNodeCount() {return nodes.size(); }

	// check links
	inline bool causesCycle(uint32_t from, uint32_t to) { return causesCycle(pinIndex[from], pinIndex[to]); }
	bool causesCycle(OtGraphPin from, OtGraphPin to);
	inline bool isLinkValid(uint32_t from, uint32_t to) { return isLinkValid(pinIndex[from], pinIndex[to]); }
	bool isLinkValid(OtGraphPin from, OtGraphPin to);

	// manipulate links
	inline OtGraphLink createLink(uint32_t from, uint32_t to, uint32_t id=0) { return createLink(pinIndex[from], pinIndex[to], id); }
	OtGraphLink createLink(OtGraphPin from, OtGraphPin to, uint32_t id=0);

	inline void deleteLink(uint32_t id) { deleteLink(linkIndex[id]); }
	void deleteLink(OtGraphLink link);
	inline void deleteLink(uint32_t from, uint32_t to) { deleteLink(pinIndex[from], pinIndex[to]); }
	void deleteLink(OtGraphPin from, OtGraphPin to);
	inline void deleteLinks(uint32_t any) { deleteLinks(pinIndex[any]); }
	void deleteLinks(OtGraphPin pin);

	inline void redirectLink(uint32_t id, uint32_t newTo) { redirectLink(linkIndex[id], newTo); }
	void redirectLink(OtGraphLink link, uint32_t newTo);

	// search for links
	inline OtGraphLink findLink(uint32_t from, uint32_t to) { return findLink(pinIndex[from], pinIndex[to]); }
	OtGraphLink findLink(OtGraphPin from, OtGraphPin to);

	// access nodes and pins
	inline OtGraphNode& getNode(uint32_t id) { return nodeIndex[id]; }
	inline OtGraphPin& getPin(uint32_t id) { return pinIndex[id]; }

	// handle selections
	void selectAll();
	void deselectAll();
	void select(uint32_t id, bool deselect=true);
	void select(const std::vector<uint32_t>& nodes, bool deselect=true);
	void select(int x1, int y1, int x2, int y2);
	void deselect(uint32_t id);
	inline bool isNodeSelected(uint32_t id) { return nodeIndex[id]->selected; }

	std::vector<uint32_t> getSelected();
	inline size_t getSelectedCount() { return getSelected().size(); }
	inline bool hasSelected() { return getSelected().size() != 0; }

	// iterate through nodes and links
	inline void eachNode(std::function<void(OtGraphNode&)> callback) {
		for (auto& node : nodes) {
			callback(node);
		}
	}

	inline void eachLink(std::function<void(OtGraphLink&)> callback) {
		for (auto& link : links) {
			callback(link);
		}
	}

	// archive/restore nodes to/from a string (support for do/undo/redo operations)
	std::string archiveNode(uint32_t node);
	std::string archiveNodes(const std::vector<uint32_t>& nodes);
	void restoreNodes(const std::string& json);
	std::vector<uint32_t> duplicateNodes(const std::string& json);

	// (re)evaluate entire graph
	void evaluate();

private:
	// properties
	OtGraphNodeFactory factory;

	std::vector<OtGraphNode> nodes;
	std::vector<OtGraphLink> links;

	std::unordered_map<uint32_t, OtGraphNode> nodeIndex;
	std::unordered_map<uint32_t, OtGraphPin> pinIndex;
	std::unordered_map<uint32_t, OtGraphLink> linkIndex;

	bool needsSorting = false;
	bool needsEvaluating = false;

	// metadata for editor
	std::string metadata = "{}";

	// (un)index a node and its pins
	void indexNode(OtGraphNode node);
	void unindexNode(OtGraphNode node);

	// helper functions for depth-first searches and topological sorting
	bool hasCycle(OtGraphNodeClass* node, OtGraphNodeClass* newTarget=0);
	bool visitNode(OtGraphNode& node, std::vector<OtGraphNode>& nodes);
	bool sortNodesTopologically(std::vector<OtGraphNode>& nodes);

	// restore a node from its JSON data
	OtGraphNode restoreNode(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);
};
