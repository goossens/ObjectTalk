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
#include <typeinfo>

#include "OtEditorTask.h"
#include "OtEntity.h"
#include "OtScene.h"


//
//	OtEditComponentTask
//

template <typename T>
class OtEditComponentTask : public OtEditorTask {
public:
	// constructor
	OtEditComponentTask(OtScene* s, OtEntity e, const std::string& o, const std::string& n) : scene(s), oldValue(o), newValue(n) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "edit component"; }

	// do action
	void perform() override {
		// nothing to be done (as it already happened)
	}

	// undo action
	void undo() override {
		// restore old value
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto& component = scene->getComponent<T>(entity);
		auto data = nlohmann::json::parse(oldValue);
		component.deserialize(data, nullptr);
	}

	// redo action
	void redo() override {
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto& component = scene->getComponent<T>(entity);
		auto data = nlohmann::json::parse(newValue);
		component.deserialize(data, nullptr);
	}

	// support task merging
	bool isMergeable(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditComponentTask>(t);
		return task && task->type == type;
	}

	void merge(std::shared_ptr<OtEditorTask> t) override {
		auto task = std::dynamic_pointer_cast<OtEditComponentTask>(t);
		newValue = task->newValue;
	}

private:
	// properties
	OtScene* scene;
	uint32_t entityUuid;
	std::string oldValue;
	std::string newValue;
	std::size_t type = typeid(T).hash_code();
};
