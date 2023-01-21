//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>

#include "OtEditor.h"


//
//	OtEditorClass::setFilePath
//

void OtEditorClass::setFilePath(const std::filesystem::path& p) {
	if (p.extension() == getFileExtension()) {
		path = p;

	} else {
		path = p;
		path.replace_extension(std::filesystem::path(getFileExtension()));
	}
}


//
//	OtEditorClass::fileExists
//

bool OtEditorClass::fileExists() {
	return std::filesystem::is_regular_file(path);
}
