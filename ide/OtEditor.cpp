//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>
#include <sstream>

#include "OtEditor.h"


//
//	OtEditorClass::OtEditorClass
//

OtEditorClass::OtEditorClass() {
	// determine editor ID
	static int nextID = 1;
	std::ostringstream name;
	name << "editor" << nextID++;
	id = name.str();
}


//
//	OtEditorClass::getShortName
//

std::string OtEditorClass::getShortName() {
	std::string shortName = std::filesystem::path(filename).filename().string();

	if (isDirty()) {
		return shortName + " *";

	} else {
		return shortName;
	}
}
