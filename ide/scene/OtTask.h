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


//
//	OtTask
//

class OtTask : public OtEditorTask {
public:
	// constructor
	OtTask() {}

	// get task name
	std::string name() { return ""; }

	// perform action
	virtual void perform() {
	}

	// undo action
	virtual void undo() {
	}

private:
	// properties
};

