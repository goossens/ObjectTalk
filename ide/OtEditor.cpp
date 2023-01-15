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
	return std::filesystem::path(filename).filename().string();
}


//
//	OtEditorClass::setFileName
//

void OtEditorClass::setFileName(const std::string& name) {
	auto path = std::filesystem::path(name);

	if (path.extension() == getFileExtension()) {
		filename = name;

	} else {
		filename = path.replace_extension(std::filesystem::path(getFileExtension()));
	}
}


//
//	OtEditorClass::fileExists
//

bool OtEditorClass::fileExists() {
	return std::filesystem::is_regular_file(std::filesystem::path(filename));
}
