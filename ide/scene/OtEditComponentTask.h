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
#include <typeinfo>

#include "OtComponents.h"
#include "OtEditorTask.h"
#include "OtEntity.h"
#include "OtScene2.h"


//
//	OtEditComponentTask
//

template <typename T>
class OtEditComponentTask : public OtEditorTask {
public:
	// constructor
	OtEditComponentTask(OtScene2 s, OtEntity e, T& n) : scene(s), newValue(n) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "change component"; }

	// perform action
	virtual void perform() {
		// save old value and store new value
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto& value = scene->getComponent<T>(entity);
		oldValue = value;
		value = newValue;
	}

	// undo action
	virtual void undo() {
		// restore old value
		auto entity = scene->getEntityFromUuid(entityUuid);
		scene->getComponent<T>(entity) = oldValue;
	}

	// support task merging
	virtual bool isMergeable(std::shared_ptr<OtEditorTask> t) {
		auto task = std::dynamic_pointer_cast<OtEditComponentTask>(t);
		return task && task->type == type;
	}

	virtual void merge(std::shared_ptr<OtEditorTask> t) {
		auto task = std::dynamic_pointer_cast<OtEditComponentTask>(t);
		newValue = task->newValue;
	}

private:
	// properties
	OtScene2 scene;
	uint32_t entityUuid;
	T oldValue;
	T newValue;
	std::size_t type = typeid(T).hash_code();
};
