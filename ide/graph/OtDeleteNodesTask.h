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
//	OtDeleteNodesTask
//

class OtDeleteNodesTask : public OtGraphEditorTask {
public:
	// constructor
	inline OtDeleteNodesTask(OtGraph* g, const std::vector<uint32_t>& n) : graph(g), nodes(n) {}

	// get task name
	std::string name() { return "delete nodes"; }

	// do action
	void perform() override {
		json = graph->archiveNodes(nodes);
		graph->deleteNodes(nodes);
		clearEditorSelection();
	}

	// undo action
	void undo() override {
		graph->restoreNodes(json);
		selectNodesInEditor(nodes);
	}

	// redo action
	void redo() override {
		graph->deleteNodes(nodes);
		clearEditorSelection();
	}

protected:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;
	std::string json;
};
