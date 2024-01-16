//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtEditNodeTask
//

class OtEditNodeTask : public OtEditorTask {
public:
	// constructor
	inline OtEditNodeTask(OtGraph* g, uint32_t n) : graph(g), node(n) {}

	// get task name
	std::string name() { return "change node"; }

	// do action
	void perform() override {
		auto targetNode = graph->getNode(node);
		oldState = targetNode->oldState;
		newState = targetNode->newState;
		targetNode->w = 0.0f;
	}

	// undo action
	void undo() override {
		auto targetNode = graph->getNode(node);
		targetNode->deserializeFromString(oldState);
		targetNode->w = 0.0f;
	}

	// redo action
	void redo() override {
		auto targetNode = graph->getNode(node);
		targetNode->deserializeFromString(newState);
		targetNode->w = 0.0f;
	}

/*
	// support task merging
	bool isMergeable(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditNodeTask>(t);
		return task && task->node == node;
	}

	void merge(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditNodeTask>(t);
		newState = task->newState;
	}
*/

private:
	// properties
	OtGraph* graph;
	uint32_t node;
	std::string oldState;
	std::string newState;
};
