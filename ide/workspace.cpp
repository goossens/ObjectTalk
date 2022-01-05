//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "workspace.h"


//
//	OtWorkspaceClass::init
//

void OtWorkspaceClass::init() {

}


//
//	OtWorkspaceClass::newFile
//

void OtWorkspaceClass::newFile() {

}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile(const std::string& filename) {

}


//
//	OtWorkspaceClass::run
//

void OtWorkspaceClass::run() {

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
