//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

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
	void perform() override {
		// create a new entity from the clipboard content
		auto entity = scene->deserializeEntity(clipboard);
		scene->assignNewEntityUuids(entity);
		json = scene->serializeEntity(entity);

		entityUuid = scene->getUuidFromEntity(entity);
		scene->moveEntityTo(scene->getEntityFromUuid(targetUuid), entity);
	}

	// undo action
	void undo() override {
		// remove pasted entity
		scene->removeEntity(scene->getEntityFromUuid(entityUuid));
	}

	// redo action
	void redo() override {
		// recreate entity (tree) as was created during "perform"
		auto entity = scene->deserializeEntity(json);
		scene->moveEntityTo(scene->getEntityFromUuid(targetUuid), entity);
	}

private:
	// properties
	OtScene* scene;
	uint32_t targetUuid;

	std::string clipboard;
	uint32_t entityUuid;

	std::string json;
};
