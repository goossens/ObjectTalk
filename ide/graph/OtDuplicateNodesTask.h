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
//	OtDuplicateNodesTask
//

class OtDuplicateNodesTask : public OtGraphEditorTask {
public:
	// constructor
	inline OtDuplicateNodesTask(OtGraph* g, const std::vector<uint32_t>& n) : graph(g), nodes(n) {}

	// get task name
	std::string name() { return "duplicate nodes"; }

	// do action
	void perform() override {
		duplicates = graph->duplicateNodes(graph->archiveNodes(nodes));
		json = graph->archiveNodes(duplicates);
		selectNodesInEditor(duplicates);
	}

	// undo action
	void undo() override {
		graph->deleteNodes(duplicates);
		selectNodesInEditor(nodes);
	}

	// redo action
	void redo() override {
		graph->restoreNodes(json);
		selectNodesInEditor(duplicates);
	}

protected:
	// properties
	OtGraph* graph;
	std::vector<uint32_t> nodes;
	std::vector<uint32_t> duplicates;
	std::string json;
};
