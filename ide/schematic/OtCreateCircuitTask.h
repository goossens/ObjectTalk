//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtEditorTask.h"
#include "OtCircuit.h"


//
//	OtCreateCircuitTask
//

class OtCreateCircuitTask : public OtEditorTask {
public:
	// constructor
	inline OtCreateCircuitTask(OtAudio* s, const std::string& tn, float _x, float _y) : audio(s), typeName(tn), x(_x), y(_y) {}

	// get task name
	std::string name() { return "create circuit"; }

	// do action
	void perform() override {
		id = audio->createCircuit(typeName, x, y)->id;
		archive = audio->archiveCircuit(id);
	}

	// undo action
	void undo() override {
		audio->deleteCircuit(id);
	}

	// redo action
	void redo() override {
		audio->restoreCircuits(archive);
	}

private:
	// properties
	OtAudio* audio;
	std::string typeName;
	float x;
	float y;
	int id = 0;
	std::string archive;
};
