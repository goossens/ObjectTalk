//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imgui.h"

#include "OtEditorTask.h"
#include "OtNode.h"


//
//	OtDragNodesTask
//

class OtDragNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtDragNodesTask(OtNodes* n, const ImVec2& o) : nodes(n), offset(o) {}

	// get task name
	std::string name() { return "drag nodes"; }

	// do action
	void perform() override {
		for (auto id : nodes->getSelected()) {
			auto node = nodes->getNode(id);
			auto oldPos = ImVec2(node->x, node->y);
			auto newPos = oldPos + offset;
			locations.emplace_back(id, oldPos, newPos);
			node->x = newPos.x;
			node->y = newPos.y;
		}
	}

	// undo action
	void undo() override {
		for (auto& location : locations) {
			auto node = nodes->getNode(location.id);
			node->x = location.oldPos.x;
			node->y = location.oldPos.y;
		}
	}

	// redo action
	void redo() override {
		for (auto& location : locations) {
			auto node = nodes->getNode(location.id);
			node->x = location.newPos.x;
			node->y = location.newPos.y;
		}
	}

private:
	// properties
	OtNodes* nodes;
	ImVec2 offset;

	struct location {
		location(uint32_t i, ImVec2 o, ImVec2 n) : id(i), oldPos(o), newPos(n) {}
		uint32_t id;
		ImVec2 oldPos;
		ImVec2 newPos;
	};

	std::vector<location> locations;
};
