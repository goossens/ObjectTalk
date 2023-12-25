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


//
//	OtPasteEntityTask
//

class OtPasteEntityTask : public OtEditorTask {
public:
	// constructor
	OtPasteEntityTask(OtScene* s, OtEntity t, std::string& c) : scene(s), clipboard(c) {
		targetUuid = scene->getUuidFromEntity(t);
	}

	// get task name
	std::string name() { return "paste entity"; }

	// do action
	virtual void perform() {
		// create an entity from the clipboard content (without preserving UUIDs, i.e. new copies)
		auto entity = scene->deserializeEntity(clipboard);
		scene->assignNewEntityUuids(entity);
		entityUuid = scene->getUuidFromEntity(entity);

		// add it to the target
		scene->moveEntityTo(scene->getEntityFromUuid(targetUuid), entity);
	}

	// undo action
	virtual void undo() {
		// remove pasted entity
		scene->removeEntity(scene->getEntityFromUuid(entityUuid));
	}

private:
	// properties
	OtScene* scene;
	uint32_t targetUuid;

	std::string clipboard;
	uint32_t entityUuid;
};

