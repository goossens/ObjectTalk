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
#include "OtNode.h"


//
//	OtEditNodeTask
//

class OtEditNodeTask : public OtEditorTask {
public:
	// constructor
	inline OtEditNodeTask(OtNodes* n, uint32_t tn) : nodes(n), node(tn) {}

	// get task name
	std::string name() { return "change node"; }

	// do action
	void perform() override {
		auto targetNode = nodes->getNode(node);
		oldState = targetNode->oldState;
		newState = targetNode->newState;
		targetNode->needsSizing = true;
	}

	// undo action
	void undo() override {
		auto targetNode = nodes->getNode(node);
		targetNode->deserializeFromString(oldState);
		targetNode->needsSizing = true;
	}

	// redo action
	void redo() override {
		auto targetNode = nodes->getNode(node);
		targetNode->deserializeFromString(newState);
		targetNode->needsSizing = true;
	}

	// support task merging
	bool isMergeable(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditNodeTask>(t);
		return task && task->node == node;
	}

	void merge(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditNodeTask>(t);
		newState = task->newState;
	}

private:
	// properties
	OtNodes* nodes;
	uint32_t node;
	std::string oldState;
	std::string newState;
};
