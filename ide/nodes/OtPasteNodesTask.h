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
#include <vector>

#include "OtEditorTask.h"
#include "OtNode.h"


//
//	OtPasteNodesTask
//

class OtPasteNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtPasteNodesTask(OtNodes* n, std::string& c) : nodes(n), clipboard(c) {}

	// get task name
	std::string name() { return "paste nodes"; }

	// do action
	void perform() override {
		// save previous selection
		previousSelection = nodes->getSelected();

		// create new nodes
		selectedNodes = nodes->duplicateNodes(clipboard);
		json = nodes->archiveNodes(selectedNodes);
		nodes->select(selectedNodes);
	}

	// undo action
	void undo() override {
		// remove created nodes
		nodes->deleteNodes(selectedNodes);

		// restore previous sections
		nodes->select(previousSelection);
	}

	// redo action
	void redo() override {
		nodes->restoreNodes(json);
		nodes->select(selectedNodes);
	}

protected:
	// properties
	OtNodes* nodes;
	std::vector<uint32_t> selectedNodes;
	std::vector<uint32_t> previousSelection;
	std::string& clipboard;
	std::string json;
};
