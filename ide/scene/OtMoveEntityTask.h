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
//	OtMoveEntityTask
//

class OtMoveEntityTask : public OtEditorTask {
public:
	// constructor
	OtMoveEntityTask(OtScene* s, OtEntity t, OtEntity e, bool b) : scene(s), before(b) {
		targetUuid = scene->getUuidFromEntity(t);
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "move entity"; }

	// do action
	virtual void perform() {
		// get information for undo
		auto target = scene->getEntityFromUuid(targetUuid);
		auto entity = scene->getEntityFromUuid(entityUuid);
		auto nextSibling = scene->getNextSibling(entity);

		if (scene->isValidEntity(nextSibling)) {
			undoTargetUuid = scene->getUuidFromEntity(nextSibling);
			undoBefore = true;

		} else {
			undoTargetUuid = scene->getUuidFromEntity(scene->getParent(entity));
			undoBefore = false;
		}

		// perform move
		if (before) {
			scene->moveEntityBefore(target, entity);

		} else {
			scene->moveEntityTo(target, entity);
		}
	}

	// undo action
	virtual void undo() {
		// undo move
		auto target = scene->getEntityFromUuid(undoTargetUuid);
		auto entity = scene->getEntityFromUuid(entityUuid);

		if (undoBefore) {
			scene->moveEntityBefore(target, entity);

		} else {
			scene->moveEntityTo(target, entity);
		}
	}

private:
	// properties
	OtScene* scene;
	uint32_t targetUuid;
	uint32_t entityUuid;
	bool before;

	uint32_t undoTargetUuid;
	bool undoBefore;
};
