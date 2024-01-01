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
//	OtDeleteNodesTask
//

class OtDeleteNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtDeleteNodesTask(OtGraph* g) : graph(g) {}

	// get task name
	std::string name() { return "delete nodes"; }

	// do action
	void perform() override {
		nodes = graph->getSelected();
		json = graph->archiveNodes(nodes);
		graph->deleteNodes(nodes);
		graph->deselectAll();
	}

	// undo action
	void undo() override {
		graph->restoreNodes(json);
		graph->select(nodes);
	}

	// redo action
	void redo() override {
		graph->deleteNodes(nodes);
		graph->deselectAll();
	}

protected:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;
	std::string json;
};
