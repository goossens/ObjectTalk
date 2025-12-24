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
#include <vector>

#include "OtDeleteCircuitsTask.h"
#include "OtCircuit.h"


//
//	OtCutCircuitsTask
//

class OtCutCircuitsTask : public OtDeleteCircuitsTask {
public:
	// constructor
	OtCutCircuitsTask(OtAudio* s, std::string& c) : OtDeleteCircuitsTask(s), clipboard(c) {}

	// get task name
	std::string name() { return "cut circuits"; }

	// do action
	void perform() override {
		// perform the delete
		OtDeleteCircuitsTask::perform();

		// save circuits to clipboard and save old content
		oldClipboard = clipboard;
		clipboard = json;
	}

	// undo action
	void undo() override {
		// undo the delete
		OtDeleteCircuitsTask::undo();

		// restore clipboard
		clipboard = oldClipboard;
	}

	// redo action
	void redo() override {
		// undo the delete
		OtDeleteCircuitsTask::redo();

		// restore clipboard
		clipboard = json;
	}

private:
	// properties
	std::string& clipboard;
	std::string oldClipboard;
};

