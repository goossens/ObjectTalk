//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtEntity.h"
#include "OtDeleteEntityTask.h"
#include "OtScene2.h"


//
//	OtCutEntityTask
//

class OtCutEntityTask : public OtDeleteEntityTask {
public:
	// constructor
	OtCutEntityTask(std::shared_ptr<OtScene2> s, OtEntity e, std::string& c) : OtDeleteEntityTask(s, e), clipboard(c) {}

	// get task name
	std::string name() { return "cut entity"; }

	// perform action
	virtual void perform() {
		// perform the delete
		OtDeleteEntityTask::perform();

		// save entity to clipboard and save old content
		oldClipboard = clipboard;
		clipboard = json;
	}

	// undo action
	virtual void undo() {
		// undo the delete
		OtDeleteEntityTask::undo();

		// restore clipboard
		clipboard = oldClipboard;
	}

private:
	// properties
	std::string& clipboard;
	std::string oldClipboard;
};

