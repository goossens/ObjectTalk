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
#include "OtGraph.h"


//
//	OtPasteNodesTask
//

class OtPasteNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtPasteNodesTask(OtGraph* g, std::string& c) : graph(g), clipboard(c) {}

	// get task name
	std::string name() { return "paste nodes"; }

	// do action
	void perform() override {
		// save previous selection
		previousSelection = graph->getSelected();

		// create new nodes
		nodes = graph->duplicateNodes(clipboard);
		json = graph->archiveNodes(nodes);
		graph->select(nodes);
	}

	// undo action
	void undo() override {
		// remove created nodes
		graph->deleteNodes(nodes);

		// restore previous sections
		graph->select(previousSelection);
	}

	// redo action
	void redo() override {
		graph->restoreNodes(json);
		graph->select(nodes);
	}

protected:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;
	std::vector<uint32_t> previousSelection;
	std::string& clipboard;
	std::string json;
};
