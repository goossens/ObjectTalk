//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtDeleteEntityTask
//

class OtDeleteEntityTask : public OtEditorTask {
public:
	// constructor
	OtDeleteEntityTask(OtScene* s, OtEntity e) : scene(s) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "delete entity"; }

	// do action
	void perform() override {
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
		json = scene->archiveEntity(entity);

		// now delete the entity
		scene->removeEntity(entity);
	}

	// undo action
	void undo() override {
		// recreate the entity
		auto entity = scene->restoreEntity(json);

		// add it back to its parent
		auto target = scene->getEntityFromUuid(undoTargetUuid);

		if (undoBefore) {
			scene->moveEntityBefore(target, entity);

		} else {
			scene->moveEntityTo(target, entity);
		}
	}

	// redo action
	void redo() override {
		auto entity = scene->getEntityFromUuid(entityUuid);
		scene->removeEntity(entity);
	}

protected:
	// properties
	OtScene* scene;
	uint32_t entityUuid;

	uint32_t undoTargetUuid;
	bool undoBefore;
	std::string json;
};
