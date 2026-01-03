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
//	OtDuplicateCircuitsTask
//

class OtDuplicateCircuitsTask : public OtEditorTask {
public:
	// constructor
	inline OtDuplicateCircuitsTask(OtAudio* s) : audio(s) {}

	// get task name
	std::string name() { return "duplicate circuits"; }

	// do action
	void perform() override {
		selectedCircuits = audio->getSelected();
		duplicates = audio->duplicateCircuits(audio->archiveCircuits(selectedCircuits));
		json = audio->archiveCircuits(duplicates);
		audio->select(duplicates);
	}

	// undo action
	void undo() override {
		audio->deleteCircuits(duplicates);
		audio->select(selectedCircuits);
	}

	// redo action
	void redo() override {
		audio->restoreCircuits(json);
		audio->select(duplicates);
	}

protected:
	// properties
	OtAudio* audio;
	std::vector<uint32_t> selectedCircuits;
	std::vector<uint32_t> duplicates;
	std::string json;
};
