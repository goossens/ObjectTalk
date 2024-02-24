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
#include "OtNode.h"


//
//	OtChangeLinkTask
//

class OtChangeLinkTask : public OtEditorTask {
public:
	// constructor
	inline OtChangeLinkTask(OtNodes* n, uint32_t f, uint32_t ot, uint32_t nt) : nodes(n), from(f), oldTo(ot), newTo(nt) {}

	// get task name
	std::string name() { return "change link"; }

	// do action
	void perform() override {
		link = nodes->findLink(from, oldTo)->id;
		nodes->redirectLink(link, newTo);
	}

	// undo action
	void undo() override {
		nodes->redirectLink(link, oldTo);
	}

	// redo action
	void redo() override {
		nodes->redirectLink(link, newTo);
	}

private:
	// properties
	OtNodes* nodes;
	uint32_t from;
	uint32_t oldTo;
	uint32_t newTo;
	uint32_t link;
};
