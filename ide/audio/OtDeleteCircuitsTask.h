//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "OtEditorTask.h"
#include "OtCircuit.h"


//
//	OtDeleteCircuitsTask
//

class OtDeleteCircuitsTask : public OtEditorTask {
public:
	// constructor
	inline OtDeleteCircuitsTask(OtAudio* s) : audio(s) {}

	// get task name
	std::string name() { return "delete circuits"; }

	// do action
	void perform() override {
		selectedCircuits = audio->getSelected();
		json = audio->archiveCircuits(selectedCircuits);
		audio->deleteCircuits(selectedCircuits);
		audio->deselectAll();
	}

	// undo action
	void undo() override {
		audio->restoreCircuits(json);
		audio->select(selectedCircuits);
	}

	// redo action
	void redo() override {
		audio->deleteCircuits(selectedCircuits);
		audio->deselectAll();
	}

protected:
	// properties
	OtAudio* audio;
	std::vector<uint32_t> selectedCircuits;
	std::string json;
};
