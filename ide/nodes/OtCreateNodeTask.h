//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtEditorTask.h"
#include "OtNode.h"


//
//	OtCreateNodeTask
//

class OtCreateNodeTask : public OtEditorTask {
public:
	// constructor
	inline OtCreateNodeTask(OtNodes* n, const std::string& tn, float _x, float _y) : nodes(n), typeName(tn), x(_x), y(_y) {}

	// get task name
	std::string name() { return "create node"; }

	// do action
	void perform() override {
		id = nodes->createNode(typeName, x, y)->id;
		archive = nodes->archiveNode(id);
	}

	// undo action
	void undo() override {
		nodes->deleteNode(id);
	}

	// redo action
	void redo() override {
		nodes->restoreNodes(archive);
	}

private:
	// properties
	OtNodes* nodes;
	std::string typeName;
	float x;
	float y;
	int id = 0;
	std::string archive;
};
