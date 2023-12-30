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

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtDuplicateNodesTask
//

class OtDuplicateNodesTask : public OtEditorTask {
public:
	// constructor
	inline OtDuplicateNodesTask(OtGraph* g) : graph(g) {}

	// get task name
	std::string name() { return "duplicate nodes"; }

	// do action
	void perform() override {
		nodes = graph->getSelected();
		duplicates = graph->duplicateNodes(graph->archiveNodes(nodes));
		json = graph->archiveNodes(duplicates);
		graph->select(duplicates);
	}

	// undo action
	void undo() override {
		graph->deleteNodes(duplicates);
		graph->select(nodes);
	}

	// redo action
	void redo() override {
		graph->restoreNodes(json);
		graph->select(duplicates);
	}

protected:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;
	std::vector<uint32_t> duplicates;
	std::string json;
};
