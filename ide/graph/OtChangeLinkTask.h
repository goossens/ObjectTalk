//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtChangeLinkTask
//

class OtChangeLinkTask : public OtEditorTask {
public:
	// constructor
	inline OtChangeLinkTask(OtGraph* g, uint32_t f, uint32_t t, uint32_t n) : graph(g), from(f), oldTo(t), newTo(n) {}

	// get task name
	std::string name() { return "change link"; }

	// do action
	void perform() override {
		link = graph->findLink(from, oldTo)->id;
		graph->redirectLink(link, newTo);
	}

	// undo action
	void undo() override {
		graph->redirectLink(link, oldTo);
	}

	// redo action
	void redo() override {
		graph->redirectLink(link, newTo);
	}

private:
	// properties
	OtGraph* graph;
	uint32_t from;
	uint32_t oldTo;
	uint32_t newTo;
	uint32_t link;
};
