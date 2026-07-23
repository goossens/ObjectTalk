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
//	OtChangeWireTask
//

class OtChangeWireTask : public OtEditorTask {
public:
	// constructor
	OtChangeWireTask(OtAudio* audio, uint32_t from, uint32_t oldTo, uint32_t newTo) :
		audio(audio), from(from), oldTo(oldTo), newTo(newTo) {}

	// get task name
	std::string name() { return "change wire"; }

	// do action
	void perform() override {
		wire = audio->findWire(from, oldTo)->id;
		audio->redirectWire(wire, newTo);
	}

	// undo action
	void undo() override {
		audio->redirectWire(wire, oldTo);
	}

	// redo action
	void redo() override {
		audio->redirectWire(wire, newTo);
	}

private:
	// properties
	OtAudio* audio;
	uint32_t from;
	uint32_t oldTo;
	uint32_t newTo;
	uint32_t wire;
};
