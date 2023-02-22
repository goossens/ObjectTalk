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
#include "OtScene2.h"


//
//	OtDeleteEntityTask
//

class OtDeleteEntityTask : public OtEditorTask {
public:
	// constructor
	OtDeleteEntityTask(std::shared_ptr<OtScene2> s, OtEntity e) : scene(s) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "delete entity"; }

	// perform action
	virtual void perform() {
		// get information for undo
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto nextSibling = scene->getNextSibling(entity);

		if (scene->isValidEntity(nextSibling)) {
			undoTargetUuid = scene->getUuidFromEntity(nextSibling);
			undoBefore = true;

		} else {
			undoTargetUuid = scene->getUuidFromEntity(scene->getParent(entity));
			undoBefore = false;
		}

		// serialize the entity to be deleted
		json = scene->serializeEntity(entity);

		// now delete the entity
		scene->removeEntity(entity);
	}

	// undo action
	virtual void undo() {
		// recreate the entity
		auto entity = scene->deserializeEntity(json);
		auto target = scene->getEntityFromUuid(undoTargetUuid);

		if (undoBefore) {
			scene->moveEntityBefore(target, entity);

		} else {
			scene->moveEntityTo(target, entity);
		}
	}

protected:
	// properties
	std::shared_ptr<OtScene2> scene;
	uint32_t entityUuid;

	uint32_t undoTargetUuid;
	bool undoBefore;
	std::string json;
};
