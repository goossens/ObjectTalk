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
//	OtCopyEntityTask
//

class OtCopyEntityTask : public OtEditorTask {
public:
	// constructor
	OtCopyEntityTask(OtScene* s, OtEntity e, std::string& c) : scene(s), clipboard(c) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "copy entity"; }

	// do action
	void perform() override {
		// serialize entity to clipboard
		oldClipboard = clipboard;
		clipboard = scene->serializeEntity(scene->getEntityFromUuid(entityUuid));
	}

	// undo action
	void undo() override {
		clipboard = oldClipboard;
	}

private:
	// properties
	OtScene* scene;
	uint32_t entityUuid;

	std::string& clipboard;
	std::string oldClipboard;
};

