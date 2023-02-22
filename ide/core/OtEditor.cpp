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
//	OtEditor::setFilePath
//

void OtEditor::setFilePath(const std::filesystem::path& p) {
	if (p.extension() == getFileExtension()) {
		path = p;

	} else {
		path = p;
		path.replace_extension(std::filesystem::path(getFileExtension()));
	}
}


//
//	OtEditor::fileExists
//

bool OtEditor::fileExists() {
	return std::filesystem::is_regular_file(path);
}
