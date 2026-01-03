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
//	OtPasteCircuitsTask
//

class OtPasteCircuitsTask : public OtEditorTask {
public:
	// constructor
	inline OtPasteCircuitsTask(OtAudio* s, std::string& c) : audio(s), clipboard(c) {}

	// get task name
	std::string name() { return "paste circuits"; }

	// do action
	void perform() override {
		// save previous selection
		previousSelection = audio->getSelected();

		// create new circuits
		selectedCircuits = audio->duplicateCircuits(clipboard);
		json = audio->archiveCircuits(selectedCircuits);
		audio->select(selectedCircuits);
	}

	// undo action
	void undo() override {
		// remove created circuits
		audio->deleteCircuits(selectedCircuits);

		// restore previous sections
		audio->select(previousSelection);
	}

	// redo action
	void redo() override {
		audio->restoreCircuits(json);
		audio->select(selectedCircuits);
	}

protected:
	// properties
	OtAudio* audio;
	std::vector<uint32_t> selectedCircuits;
	std::vector<uint32_t> previousSelection;
	std::string& clipboard;
	std::string json;
};
