//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
		oldState = graph->getNode(node)->oldState;
		newState = graph->getNode(node)->newState;
	}

	// undo action
	void undo() override {
		auto targetNode = graph->getNode(node);
		targetNode->OtGraphNodeClass::deserializeFromString(oldState);
		targetNode->needsRunning = true;
	}

	// redo action
	void redo() override {
		auto targetNode = graph->getNode(node);
		targetNode->OtGraphNodeClass::deserializeFromString(newState);
		targetNode->needsRunning = true;
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
	OtGraph* graph;
	uint32_t node;
	std::string oldState;
	std::string newState;
};
