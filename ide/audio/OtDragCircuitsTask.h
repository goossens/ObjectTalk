//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imgui.h"

#include "OtEditorTask.h"
#include "OtCircuit.h"


//
//	OtDragCircuitsTask
//

class OtDragCircuitsTask : public OtEditorTask {
public:
	// constructor
	inline OtDragCircuitsTask(OtAudio* s, const ImVec2& o) : audio(s), offset(o) {}

	// get task name
	std::string name() { return "drag circuits"; }

	// do action
	void perform() override {
		for (auto id : audio->getSelected()) {
			auto circuit = audio->getCircuit(id);
			auto oldPos = ImVec2(circuit->x, circuit->y);
			auto newPos = oldPos + offset;
			locations.emplace_back(id, oldPos, newPos);
			circuit->x = newPos.x;
			circuit->y = newPos.y;
		}
	}

	// undo action
	void undo() override {
		for (auto& location : locations) {
			auto circuit = audio->getCircuit(location.id);
			circuit->x = location.oldPos.x;
			circuit->y = location.oldPos.y;
		}
	}

	// redo action
	void redo() override {
		for (auto& location : locations) {
			auto circuit = audio->getCircuit(location.id);
			circuit->x = location.newPos.x;
			circuit->y = location.newPos.y;
		}
	}

private:
	// properties
	OtAudio* audio;
	ImVec2 offset;

	struct location {
		location(uint32_t i, ImVec2 o, ImVec2 n) : id(i), oldPos(o), newPos(n) {}
		uint32_t id;
		ImVec2 oldPos;
		ImVec2 newPos;
	};

	std::vector<location> locations;
};
