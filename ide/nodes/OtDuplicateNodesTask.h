//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtDuplicateNodesTask
//

class OtDuplicateNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtDuplicateNodesTask(OtNodes* n) : nodes(n) {}

	// get task name
	std::string name() { return "duplicate nodes"; }

	// do action
	void perform() override {
		selectedNodes = nodes->getSelected();
		duplicates = nodes->duplicateNodes(nodes->archiveNodes(selectedNodes));
		json = nodes->archiveNodes(duplicates);
		nodes->select(duplicates);
	}

	// undo action
	void undo() override {
		nodes->deleteNodes(duplicates);
		nodes->select(selectedNodes);
	}

	// redo action
	void redo() override {
		nodes->restoreNodes(json);
		nodes->select(duplicates);
	}

protected:
	// properties
	OtNodes* nodes;
	std::vector<uint32_t> selectedNodes;
	std::vector<uint32_t> duplicates;
	std::string json;
};
