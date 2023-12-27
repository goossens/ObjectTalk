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
#include <vector>

#include "OtGraph.h"
#include "OtGraphEditorTask.h"


//
//	OtCopyNodesTask
//

class OtCopyNodesTask : public OtGraphEditorTask {
public:
	// constructor
	inline OtCopyNodesTask(OtGraph* g, const std::vector<uint32_t>& n, std::string& c) : graph(g), nodes(n), clipboard(c) {}

	// get task name
	std::string name() { return "copy nodes"; }

	// do action
	void perform() override {
		// serialize entity to clipboard
		oldClipboard = clipboard;
		clipboard = graph->archiveNodes(nodes);
	}

	// undo action
	void undo() override {
		// restore clipboard
		clipboard = oldClipboard;
	}

	// redo action
	void redo() override {
	}

protected:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;
	std::string& clipboard;
	std::string oldClipboard;
};
