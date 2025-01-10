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

#include "OtEditorTask.h"
#include "OtScene.h"


//
//	OtEditPostProcessingTask
//

class OtEditPostProcessingTask : public OtEditorTask {
public:
	// constructor
	OtEditPostProcessingTask(OtScene* s, const std::string& o, const std::string& n) : scene(s), oldValue(o), newValue(n) {}

	// get task name
	std::string name() { return "edit post processing"; }

	// do action
	void perform() override {
		// nothing to be done (as it already happened)
	}

	// undo action
	void undo() override {
		// restore old value
		auto data = nlohmann::json::parse(oldValue);
		scene->getPostProcessing().deserialize(data, nullptr);
	}

	// redo action
	void redo() override {
		// restore new value
		auto data = nlohmann::json::parse(newValue);
		scene->getPostProcessing().deserialize(data, nullptr);
	}

	// support task merging
	bool isMergeable(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditPostProcessingTask>(t);
		return task != nullptr;
	}

	void merge(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditPostProcessingTask>(t);
		newValue = task->newValue;
	}

private:
	// properties
	OtScene* scene;
	std::string oldValue;
	std::string newValue;
};
