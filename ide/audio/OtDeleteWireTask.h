//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
//	OtDeleteWireTask
//

class OtDeleteWireTask : public OtEditorTask {
public:
	// constructor
	inline OtDeleteWireTask(OtAudio* s, uint32_t f, uint32_t t) : audio(s), from(f), to(t) {}

	// get task name
	std::string name() { return "delete wire"; }

	// do action
	void perform() override {
		// create the wire
		wire = audio->findWire(from, to)->id;
		audio->deleteWire(wire);
	}

	// undo action
	void undo() override {
		audio->createWire(from, to, wire);
	}

	// redo action
	void redo() override {
		audio->deleteWire(wire);
	}

private:
	// properties
	OtAudio* audio;
	uint32_t from;
	uint32_t to;
	uint32_t wire;
};
