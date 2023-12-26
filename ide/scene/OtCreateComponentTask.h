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
#include "OtEntity.h"
#include "OtScene.h"


//
//	OtCreateComponentTask
//

template <typename T>
class OtCreateComponentTask : public OtEditorTask {
public:
	// constructor
	OtCreateComponentTask(OtScene* s, OtEntity e) : scene(s) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "create component"; }

	// do action
	void perform () override {
		// get the target entity
		auto entity = scene->getEntityFromUuid(entityUuid);

		// add appropriate component
		scene->addComponent<T>(entity);
	}

	// undo action
	void undo() override {
		// get the target entity
		auto entity = scene->getEntityFromUuid(entityUuid);

		// remove appropriate component
		scene->removeComponent<T>(entity);
	}

private:
	// properties
	OtScene* scene;
	uint32_t entityUuid;
};
