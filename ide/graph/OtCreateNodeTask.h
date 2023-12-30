//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtCreateNodeTask
//

class OtCreateNodeTask : public OtEditorTask {
public:
	// constructor
	inline OtCreateNodeTask(OtGraph* g, const std::string& tn, float _x, float _y) : graph(g), typeName(tn), x(_x), y(_y) {}

	// get task name
	std::string name() { return "create node"; }

	// do action
	void perform() override {
		id = graph->createNode(typeName, x, y)->id;
		archive = graph->archiveNode(id);
	}

	// undo action
	void undo() override {
		graph->deleteNode(id);
	}

	// redo action
	void redo() override {
		graph->restoreNodes(archive);
	}

private:
	// properties
	OtGraph* graph;
	std::string typeName;
	float x;
	float y;
	int id = 0;
	std::string archive;
};
