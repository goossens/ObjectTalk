//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <vector>

#include "OtEditorTask.h"


//
//	OtTaskManager
//

class OtTaskManager {
public:
	// perform a task
	void perform(std::shared_ptr<OtEditorTask> task) {
		task->perform();
		undoStack.push_back(task);
		redoStack.clear();
	}

	// undo a task
	void undo() {
		auto task = undoStack.back();
		task->undo();
		undoStack.pop_back();
		redoStack.push_back(task);
	}

	// redo a task
	void redo() {
		auto task = redoStack.back();
		task->perform();
		redoStack.pop_back();
		undoStack.push_back(task);
	}

	// see if we can undo or redo
	bool canUndo() { return undoStack.size() > 0; };
	bool canRedo() { return redoStack.size() > 0; };

	// get counts
	size_t getUndoCount() { return undoStack.size(); }
	size_t getRedoCount() { return redoStack.size(); }

private:
	// to keep track of tasks
	std::vector<std::shared_ptr<OtEditorTask>> undoStack;
	std::vector<std::shared_ptr<OtEditorTask>> redoStack;
};
