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

	// perform action
	virtual void perform() {
		// duplicate the target entity
		auto target = scene->getEntityFromUuid(targetUuid);
		auto entity = scene->deserializeEntity(scene->serializeEntity(target));
		scene->assignNewEntityUUids(entity);
		entityUuid = scene->getUuidFromEntity(entity);

		// add duplicate after target
		scene->insertEntityAfter(target, entity);
	}

	// undo action
	virtual void undo() {
		// remove duplicated entity
		scene->removeEntity(scene->getEntityFromUuid(entityUuid));
	}

private:
	// properties
	OtScene* scene;
	uint32_t targetUuid;
	uint32_t entityUuid;
};

