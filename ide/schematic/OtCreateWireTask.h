//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtEditorTask.h"
#include "OtCircuit.h"


//
//	OtCreateWireTask
//

class OtCreateWireTask : public OtEditorTask {
public:
	// constructor
	inline OtCreateWireTask(OtAudio* s, uint32_t f, uint32_t t) : audio(s), from(f), to(t) {}

	// get task name
	std::string name() { return "create wire"; }

	// do action
	void perform() override {
		// create the wire
		wire = audio->createWire(from, to)->id;
	}

	// undo action
	void undo() override {
		audio->deleteWire(wire);
	}

	// redo action
	void redo() override {
		audio->createWire(from, to, wire);
	}

private:
	// properties
	OtAudio* audio;
	uint32_t from;
	uint32_t to;
	uint32_t wire;
};
