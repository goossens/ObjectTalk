//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtEditorTask.h"
#include "OtNode.h"


//
//	OtCopyNodesTask
//

class OtCopyNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtCopyNodesTask(OtNodes* n, std::string& c) : nodes(n), clipboard(c) {}

	// get task name
	std::string name() { return "copy nodes"; }

	// do action
	void perform() override {
		// serialize nodes to clipboard
		oldClipboard = clipboard;
		json = nodes->archiveNodes(nodes->getSelected());
		clipboard = json;
	}

	// undo action
	void undo() override {
		// restore clipboard
		clipboard = oldClipboard;
	}

	// redo action
	void redo() override {
		clipboard = json;
	}

protected:
	// properties
	OtNodes* nodes;
	std::string& clipboard;
	std::string oldClipboard;
	std::string json;
};
