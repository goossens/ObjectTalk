//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "glm/glm.hpp"


//
//	Functions
//

// create a header with specified width
void OtUiHeader(const char* label, float width);

// create an input field based on a std::string
bool OtUiInputText(const char* label, std::string& value);

// create a field to edit glm vectors
bool OtUiEditVec3(const char* label, glm::vec3& vector, float speed, float minv, float maxv);
bool OtUiEditVec4(const char* label, glm::vec4& vector, float speed, float minv, float maxv);

// create a file path field with file selector popup
bool OtUiFileSelector(const char* label, std::filesystem::path& path);

// create a splitter widget
void OtUiSplitterVertical(float* size, float minSize, float maxSize);
void OtUiSplitterHorizontal(float* size, float minSize, float maxSize);
