//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	OtCreateLinkTask(OtNodes* nodes, uint32_t oldFrom, uint32_t from, uint32_t to) :
		nodes(nodes), oldFrom(oldFrom), from(from), to(to) {}

	// get task name
	std::string name() { return "create link"; }

	// do action
	void perform() override {
		// remove old link (if required)
		if (oldFrom) {
			oldLink = nodes->findLink(oldFrom, to)->id;
			nodes->deleteLink(oldFrom, to);
		}

		// create the link
		link = nodes->createLink(from, to)->id;
	}

	// undo action
	void undo() override {
		nodes->deleteLink(link);

		if (oldFrom) {
			nodes->createLink(oldFrom, to, oldLink);
		}
	}

	// redo action
	void redo() override {
		if (oldFrom) {
			nodes->deleteLink(oldFrom, to);
		}

		nodes->createLink(from, to, link);
	}

private:
	// properties
	OtNodes* nodes;
	uint32_t oldFrom;
	uint32_t from;
	uint32_t to;
	uint32_t oldLink;
	uint32_t link;
};
