//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAudio.h"

#include "OtEditorTask.h"


//
//	OtCreateWireTask
//

class OtCreateWireTask : public OtEditorTask {
public:
	// constructor
	OtCreateWireTask(OtAudio* audio, uint32_t oldFrom, uint32_t from, uint32_t to) :
		audio(audio), oldFrom(oldFrom), from(from), to(to) {}

	// get task name
	std::string name() { return "create wire"; }

	// do action
	void perform() override {
		// remove old wire (if required)
		if (oldFrom) {
			oldWire = audio->findWire(oldFrom, to)->id;
			audio->deleteWire(oldFrom, to);
		}

		// create the wire
		wire = audio->createWire(from, to)->id;
	}

	// undo action
	void undo() override {
		audio->deleteWire(wire);

		if (oldFrom) {
			audio->createWire(oldFrom, to, oldWire);
		}
	}

	// redo action
	void redo() override {
		if (oldFrom) {
			audio->deleteWire(oldFrom, to);
		}

		audio->createWire(from, to, wire);
	}

private:
	// properties
	OtAudio* audio;
	uint32_t oldFrom;
	uint32_t from;
	uint32_t to;
	uint32_t oldWire;
	uint32_t wire;
};
