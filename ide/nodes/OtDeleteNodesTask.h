//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "OtEditorTask.h"
#include "OtNode.h"


//
//	OtDeleteNodesTask
//

class OtDeleteNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtDeleteNodesTask(OtNodes* n) : nodes(n) {}

	// get task name
	std::string name() { return "delete nodes"; }

	// do action
	void perform() override {
		selectedNodes = nodes->getSelected();
		json = nodes->archiveNodes(selectedNodes);
		nodes->deleteNodes(selectedNodes);
		nodes->deselectAll();
	}

	// undo action
	void undo() override {
		nodes->restoreNodes(json);
		nodes->select(selectedNodes);
	}

	// redo action
	void redo() override {
		nodes->deleteNodes(selectedNodes);
		nodes->deselectAll();
	}

protected:
	// properties
	OtNodes* nodes;
	std::vector<uint32_t> selectedNodes;
	std::string json;
};
