//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtNodesFactory.h"
#include "OtNodesLink.h"
#include "OtNode.h"
#include "OtNodesPin.h"


//
//	OtNodes
//

class OtNodes {
public:
	// destructor
	~OtNodes();

	// register a node type
	template <typename T>
	inline void registerNodeType(const char* category, const char* name) {
		OtNodesFactory::registerType(
			category,
			name,
			[]() {
				auto node = std::make_shared<T>();
				node->configure();
				return node;
			});
	}

	// clear the entire nodes
	void clear();

	// load and save nodes
	void load(const std::string& path);
	void loadFromString(const std::string& string, std::string& basedir);
	void save(const std::string& path);

	// manipulate nodes
	OtNode createNode(const std::string& name, float x, float y);
	inline void deleteNode(uint32_t id) { deleteNode(nodeIndex[id]); }
	void deleteNode(OtNode node);
	void deleteNodes(const std::vector<uint32_t>& nodes);
	inline size_t getNodeCount() {return nodes.size(); }

	// check links
	inline bool causesCycle(uint32_t from, uint32_t to) { return causesCycle(pinIndex[from], pinIndex[to]); }
	bool causesCycle(OtNodesPin from, OtNodesPin to);
	inline bool isLinkValid(uint32_t from, uint32_t to) { return isLinkValid(pinIndex[from], pinIndex[to]); }
	bool isLinkValid(OtNodesPin from, OtNodesPin to);

	// manipulate links
	inline OtNodesLink createLink(uint32_t from, uint32_t to, uint32_t id=0) { return createLink(pinIndex[from], pinIndex[to], id); }
	OtNodesLink createLink(OtNodesPin from, OtNodesPin to, uint32_t id=0);

	inline void deleteLink(uint32_t id) { deleteLink(linkIndex[id]); }
	void deleteLink(OtNodesLink link);
	inline void deleteLink(uint32_t from, uint32_t to) { deleteLink(pinIndex[from], pinIndex[to]); }
	void deleteLink(OtNodesPin from, OtNodesPin to);
	inline void deleteLinks(uint32_t any) { deleteLinks(pinIndex[any]); }
	void deleteLinks(OtNodesPin pin);

	inline void redirectLink(uint32_t id, uint32_t newTo) { redirectLink(linkIndex[id], newTo); }
	void redirectLink(OtNodesLink link, uint32_t newTo);

	// search for links
	inline OtNodesLink findLink(uint32_t from, uint32_t to) { return findLink(pinIndex[from], pinIndex[to]); }
	OtNodesLink findLink(OtNodesPin from, OtNodesPin to);

	// access nodes and pins
	inline OtNode& getNode(uint32_t id) { return nodeIndex[id]; }
	inline OtNodesPin& getPin(uint32_t id) { return pinIndex[id]; }

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
	inline bool hasMultipleSelected() { return getSelected().size() > 1; }

	// iterate through nodes and links
	inline void eachNode(std::function<void(OtNode&)> callback) {
		for (auto& node : nodes) {
			callback(node);
		}
	}

	inline void eachLink(std::function<void(OtNodesLink&)> callback) {
		for (auto& link : links) {
			callback(link);
		}
	}

	// archive/restore nodes to/from a string (support for do/undo/redo operations)
	std::string archiveNode(uint32_t node);
	std::string archiveNodes(const std::vector<uint32_t>& nodes);
	void restoreNodes(const std::string& json);
	std::vector<uint32_t> duplicateNodes(const std::string& json);

	// (re)evaluate entire nodes
	void evaluate();

private:
	// properties
	std::vector<OtNode> nodes;
	std::vector<OtNodesLink> links;

	std::unordered_map<uint32_t, OtNode> nodeIndex;
	std::unordered_map<uint32_t, OtNodesPin> pinIndex;
	std::unordered_map<uint32_t, OtNodesLink> linkIndex;

	bool needsSorting = false;
	bool needsEvaluating = false;

	// metadata for editor
	std::string metadata = "{}";

	// (un)index a node and its pins
	void indexNode(OtNode node);
	void unindexNode(OtNode node);

	// helper functions for depth-first searches and topological sorting
	bool hasCycle(OtNodeClass* node, OtNodeClass* newTarget=0);
	bool visitNode(OtNode& node, std::vector<OtNode>& nodes);
	bool sortNodesTopologically();
	void classifyLinks();

	// restore a node from its JSON data
	OtNode restoreNode(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);
};
