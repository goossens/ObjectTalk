//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtEditorTask.h"
#include "OtEntity.h"
#include "OtScene.h"


//
//	OtDeleteComponentTask
//

template <typename T>
class OtDeleteComponentTask : public OtEditorTask {
public:
	// constructor
	OtDeleteComponentTask(OtScene* s, OtEntity e) : scene(s) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "delete component"; }

	// do action
	void perform() override {
		// get the target entity and component
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto& component = scene->getComponent<T>(entity);

		// save and delete component
		oldValue = component.serialize(nullptr).dump();
		scene->removeComponent<T>(entity);
	}

	// undo action
	void undo() override {
		// restore old value
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto& component = scene->addComponent<T>(entity);
		auto data = nlohmann::json::parse(oldValue);
		component.deserialize(data, nullptr);
	}

private:
	// properties
	OtScene* scene;
	uint32_t entityUuid;
	std::string oldValue;
};
