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
	OtCopyEntityTask(std::shared_ptr<OtScene> s, OtEntity e, std::string& c) : scene(s), clipboard(c) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "copy entity"; }

	// perform action
	virtual void perform() {
		// serialize entity to clipboard
		oldClipboard = clipboard;
		clipboard = scene->serializeEntity(scene->getEntityFromUuid(entityUuid));
	}

	// undo action
	virtual void undo() {
		clipboard = oldClipboard;
	}

private:
	// properties
	std::shared_ptr<OtScene> scene;
	uint32_t entityUuid;

	std::string& clipboard;
	std::string oldClipboard;
};

