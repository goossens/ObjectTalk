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
//	OtCopyEntityTask
//

class OtCopyEntityTask : public OtEditorTask {
public:
	// constructor
	OtCopyEntityTask(OtScene2 s, OtEntity e, std::string& c) : scene(s), clipboard(c) {
		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "copy entity"; }

	// perform action
	virtual void perform() {
		// serialize entity to clipboard
		oldClipboard = clipboard;
		clipboard = OtEntitySerializeToString(scene, scene->getEntityFromUuid(entityUuid));
	}

	// undo action
	virtual void undo() {
		clipboard = oldClipboard;
	}

private:
	// properties
	OtScene2 scene;
	uint32_t entityUuid;

	std::string& clipboard;
	std::string oldClipboard;
};

