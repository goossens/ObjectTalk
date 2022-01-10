//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "application.h"

#include "workspace.h"


//
//	OtWorkspaceClass::init
//

void OtWorkspaceClass::init() {
	OtApplicationClass::instance()->atrender(std::bind(&OtWorkspaceClass::render, this));
}


//
//	OtWorkspaceClass::newFile
//

void OtWorkspaceClass::newFile() {
	editors.push_back(OtEditorClass::create());
}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile(const std::string& filename) {
	editors.push_back(OtEditorClass::create(filename));
}


//
//	OtWorkspaceClass::run
//

void OtWorkspaceClass::run() {
	OtApplicationClass::instance()->run("ObjectTalk IDE");
}


//
//	OtWorkspaceClass::render
//

void OtWorkspaceClass::render() {
	// refresh all editors
	for (auto& editor : editors) {
		editor->render();
	}
}


//
//	OtWorkspaceClass::getMeta
//

OtType OtWorkspaceClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtWorkspaceClass>(
			"Workspace",
			OtIdeClass::getMeta(),
			[]() { return (OtObject) OtWorkspaceClass::instance(); });
	}

	return type;
}


//
//	OtWorkspaceClass::create
//

OtWorkspace OtWorkspaceClass::create() {
	return OtWorkspaceClass::instance();
}
