//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <limits>

#include "imgui.h"

#include "OtEditorTask.h"
#include "OtNode.h"


//
//	OtAlignNodesTask
//

class OtAlignNodesTask : public OtEditorTask {
public:
	// alignment types
	enum class Alignment {
		left,
		center,
		right,
		top,
		middle, bottom
	};

	// constructor
	inline OtAlignNodesTask(OtNodes* n, Alignment a) : nodes(n), alignment(a) {}

	// get task name
	std::string name() { return "align nodes"; }

	// do action
	inline void perform() override {
		for (auto id : nodes->getSelected()) {
			auto node = nodes->getNode(id);
			locations.emplace_back(id, ImVec2(node->x, node->y), ImVec2(node->w, node->h));
		}

		switch (alignment) {
			case Alignment::left: alignLeft(); break;
			case Alignment::center: alignCenter(); break;
			case Alignment::right: alignRight(); break;
			case Alignment::top: alignTop(); break;
			case Alignment::middle: alignMiddle(); break;
			case Alignment::bottom: alignBottom(); break;
		}

		for (auto& location : locations) {
			auto node = nodes->getNode(location.id);
			node->x = location.newPos.x;
			node->y = location.newPos.y;
		}
	}

	// undo action
	inline void undo() override {
		for (auto& location : locations) {
			auto node = nodes->getNode(location.id);
			node->x = location.oldPos.x;
			node->y = location.oldPos.y;
		}
	}

	// redo action
	inline void redo() override {
		for (auto& location : locations) {
			auto node = nodes->getNode(location.id);
			node->x = location.newPos.x;
			node->y = location.newPos.y;
		}
	}

private:
	// properties
	OtNodes* nodes;
	Alignment alignment;

	struct location {
		location(uint32_t i, ImVec2 o, ImVec2 s) : id(i), oldPos(o), oldSize(s), newPos(o) {}
		uint32_t id;
		ImVec2 oldPos;
		ImVec2 oldSize;
		ImVec2 newPos;
	};

	std::vector<location> locations;

	// alignment algorithms
	inline void alignLeft() {
		float left = std::numeric_limits<float>::max();

		for (auto& location : locations) {
			left = std::min(left, location.oldPos.x);
		}

		for (auto& location : locations) {
			location.newPos.x = left;
		}
	}

	inline void alignCenter() {
		float center = 0.0f;

		for (auto& location : locations) {
			center += location.oldPos.x + location.oldSize.x / 2.0f;
		}

		center = std::floor(center / locations.size());

		for (auto& location : locations) {
			location.newPos.x += center - (location.oldPos.x + location.oldSize.x / 2.0f);
		}
	}

	inline void alignRight() {
		float right = std::numeric_limits<float>::lowest();

		for (auto& location : locations) {
			right = std::max(right, location.oldPos.x + location.oldSize.x);
		}

		for (auto& location : locations) {
			location.newPos.x = right - location.oldSize.x;
		}
	}

	inline void alignTop() {
		float top = std::numeric_limits<float>::max();

		for (auto& location : locations) {
			top = std::min(top, location.oldPos.y);
		}

		for (auto& location : locations) {
			location.newPos.y = top;
		}
	}

	inline void alignMiddle() {
		float middle = 0.0f;

		for (auto& location : locations) {
			middle += location.oldPos.y + location.oldSize.y / 2.0f;
		}

		middle = std::floor(middle / locations.size());

		for (auto& location : locations) {
			location.newPos.y += middle - (location.oldPos.y + location.oldSize.y / 2.0f);
		}
	}

	inline void alignBottom() {
		float bottom = std::numeric_limits<float>::lowest();

		for (auto& location : locations) {
			bottom = std::max(bottom, location.oldPos.y + location.oldSize.y);
		}

		for (auto& location : locations) {
			location.newPos.y = bottom - location.oldSize.y;
		}
	}
};
