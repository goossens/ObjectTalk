//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtCreateLinkTask
//

class OtCreateLinkTask : public OtEditorTask {
public:
	// constructor
	inline OtCreateLinkTask(OtNodes* n, uint32_t f, uint32_t t) : nodes(n), from(f), to(t) {}

	// get task name
	std::string name() { return "create link"; }

	// do action
	void perform() override {
		// create the link
		link = nodes->createLink(from, to)->id;
	}

	// undo action
	void undo() override {
		nodes->deleteLink(link);
	}

	// redo action
	void redo() override {
		nodes->createLink(from, to, link);
	}

private:
	// properties
	OtNodes* nodes;
	uint32_t from;
	uint32_t to;
	uint32_t link;
};
