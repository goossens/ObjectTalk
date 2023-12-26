//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "imgui.h"
#include "imgui_node_editor.h"

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtDragNodesTask
//

class OtDragNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtDragNodesTask(OtGraph* g, const std::vector<uint32_t>& n) : graph(g), nodes(n) {}

	// get task name
	std::string name() { return "drag nodes"; }

	// do action
	void perform() override {
		// the drag already happened in the editor
		// so we just need to get the old and new locations
		// and update the location in the graph
		for (auto id : nodes) {
			auto node = graph->getNode(id);
			auto newPos = ax::NodeEditor::GetNodePosition(id);
			locations.emplace_back(id, ImVec2(node->x, node->y), newPos);
			node->x = newPos.x;
			node->y = newPos.y;
		}
	}

	// undo action
	void undo() override {
		for (auto& location : locations) {
			auto node = graph->getNode(location.id);
			node->x = location.oldPos.x;
			node->y = location.oldPos.y;
			ax::NodeEditor::SetNodePosition(location.id, location.oldPos);
		}
	}

	// redo action
	void redo() override {
		for (auto& location : locations) {
			auto node = graph->getNode(location.id);
			node->x = location.newPos.x;
			node->y = location.newPos.y;
			ax::NodeEditor::SetNodePosition(location.id, location.newPos);
		}
	}

private:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;

	struct location {
		location(uint32_t i, ImVec2 o, ImVec2 n) : id(i), oldPos(o), newPos(n) {}
		uint32_t id;
		ImVec2 oldPos;
		ImVec2 newPos;
	};

	std::vector<location> locations;
};
