//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtEditorTask.h"
#include "OtCircuit.h"


//
//	OtCopyCircuitsTask
//

class OtCopyCircuitsTask : public OtEditorTask {
public:
	// constructor
	inline OtCopyCircuitsTask(OtAudio* s, std::string& c) : audio(s), clipboard(c) {}

	// get task name
	std::string name() { return "copy circuits"; }

	// do action
	void perform() override {
		// serialize circuits to clipboard
		oldClipboard = clipboard;
		json = audio->archiveCircuits(audio->getSelected());
		clipboard = json;
	}

	// undo action
	void undo() override {
		// restore clipboard
		clipboard = oldClipboard;
	}

	// redo action
	void redo() override {
		clipboard = json;
	}

protected:
	// properties
	OtAudio* audio;
	std::string& clipboard;
	std::string oldClipboard;
	std::string json;
};
