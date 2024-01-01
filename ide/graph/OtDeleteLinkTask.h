//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtDeleteLinkTask
//

class OtDeleteLinkTask : public OtEditorTask {
public:
	// constructor
	inline OtDeleteLinkTask(OtGraph* g, uint32_t f, uint32_t t) : graph(g), from(f), to(t) {}

	// get task name
	std::string name() { return "delete link"; }

	// do action
	void perform() override {
		// create the link
		link = graph->findLink(from, to)->id;
		graph->deleteLink(link);
	}

	// undo action
	void undo() override {
		graph->createLink(from, to, link);
	}

	// redo action
	void redo() override {
		graph->deleteLink(link);
	}

private:
	// properties
	OtGraph* graph;
	uint32_t from;
	uint32_t to;
	uint32_t link;
};
