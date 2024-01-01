//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtCopyNodesTask
//

class OtCopyNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtCopyNodesTask(OtGraph* g, std::string& c) : graph(g), clipboard(c) {}

	// get task name
	std::string name() { return "copy nodes"; }

	// do action
	void perform() override {
		// serialize nodes to clipboard
		oldClipboard = clipboard;
		json = graph->archiveNodes(graph->getSelected());
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
	OtGraph* graph;
	std::string& clipboard;
	std::string oldClipboard;
	std::string json;
};
