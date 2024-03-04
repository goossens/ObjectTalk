//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtDuplicateEntityTask
//

class OtDuplicateEntityTask : public OtEditorTask {
public:
	// constructor
	OtDuplicateEntityTask(OtScene* s, OtEntity e) : scene(s) {
		targetUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return ""; }

	// do action
	void perform() override {
		// duplicate the target entity
		auto target = scene->getEntityFromUuid(targetUuid);
		auto entity = scene->duplicateEntity(scene->archiveEntity(target));
		json = scene->archiveEntity(entity);
		entityUuid = scene->getUuidFromEntity(entity);

		// add duplicate after target
		scene->insertEntityAfter(target, entity);
	}

	// undo action
	void undo() override {
		// remove duplicated entity
		scene->removeEntity(scene->getEntityFromUuid(entityUuid));
	}

	// redo action
	void redo() override {
		// recreate entity (tree) as was created during "perform"
		auto entity = scene->restoreEntity(json);
		scene->insertEntityAfter(scene->getEntityFromUuid(targetUuid), entity);
	}

private:
	// properties
	OtScene* scene;
	uint32_t targetUuid;
	uint32_t entityUuid;

	std::string json;
};

