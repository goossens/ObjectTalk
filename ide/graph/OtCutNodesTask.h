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

#include "OtDeleteNodesTask.h"
#include "OtGraph.h"



//
//	OtCutNodesTask
//

class OtCutNodesTask : public OtDeleteNodesTask {
public:
	// constructor
	OtCutNodesTask(OtGraph* g, const std::vector<uint32_t>& n, std::string& c) : OtDeleteNodesTask(g, n), clipboard(c) {}

	// get task name
	std::string name() { return "cut nodes"; }

	// do action
	void perform() override {
		// perform the delete
		OtDeleteNodesTask::perform();

		// save nodes to clipboard and save old content
		oldClipboard = clipboard;
		clipboard = json;
	}

	// undo action
	void undo() override {
		// undo the delete
		OtDeleteNodesTask::undo();

		// restore clipboard
		clipboard = oldClipboard;
	}

	// redo action
	void redo() override {
		// undo the delete
		OtDeleteNodesTask::redo();

		// restore clipboard
		clipboard = json;
	}

private:
	// properties
	std::string& clipboard;
	std::string oldClipboard;
};

