//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>


//
//	OtEditorTask
//

class OtEditorTask {
public:
	// destructor
	virtual ~OtEditorTask() {}

	// get task name
	std::string name() { return ""; }

	// perform/undo action
	virtual void perform() {}
	virtual void undo() {}
	virtual void redo() { perform(); }

	// support task merging
	virtual bool isMergeable(std::shared_ptr<OtEditorTask> /* task */) { return false; }
	virtual void merge(std::shared_ptr<OtEditorTask> /* task */) {}
};
