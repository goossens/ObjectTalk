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
//	OtEditCircuitTask
//

class OtEditCircuitTask : public OtEditorTask {
public:
	// constructor
	inline OtEditCircuitTask(OtAudio* s, uint32_t tn) : audio(s), circuit(tn) {}

	// get task name
	std::string name() { return "change circuit"; }

	// do action
	void perform() override {
		auto targetCircuit = audio->getCircuit(circuit);
		oldState = targetCircuit->oldState;
		newState = targetCircuit->newState;
		targetCircuit->needsSizing = true;
	}

	// undo action
	void undo() override {
		auto targetCircuit = audio->getCircuit(circuit);
		targetCircuit->deserializeFromString(oldState);
		targetCircuit->needsSizing = true;
	}

	// redo action
	void redo() override {
		auto targetCircuit = audio->getCircuit(circuit);
		targetCircuit->deserializeFromString(newState);
		targetCircuit->needsSizing = true;
	}

	// support task merging
	bool isMergeable(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditCircuitTask>(t);
		return task && task->circuit == circuit;
	}

	void merge(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditCircuitTask>(t);
		newState = task->newState;
	}

private:
	// properties
	OtAudio* audio;
	uint32_t circuit;
	std::string oldState;
	std::string newState;
};
