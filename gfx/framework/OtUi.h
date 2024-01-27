//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"
#include "imgui.h"


//
//	Constants
//

enum {
	uiDefaultFont = 0,
	uiEditorFont
};

#if __APPLE__
#define OT_UI_SHORTCUT "Cmd-"
#else
#define OT_UI_SHORTCUT "Ctrl-"
#endif


//
//	Functions
//

// split am ImGui label into a label and the ID
void OtUiSplitLabel(const char* text, std::string& label, std::string& id);

// render text centered in the current (child) window
void OtUiCenteredText(const char* text);

// create a header with specified width
void OtUiHeader(const char* label, float width=0.0f);

// create a readonly text filed
void OtUiReadonlyText(const char* label, std::string& value);

// create an input field based on a std::string
bool OtUiInputText(const char* label, std::string& value, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None);

// create a field to edit glm vectors
bool OtUiEditVec3(const char* label, glm::vec3& vector, float speed, float minv, float maxv);
bool OtUiEditVec4(const char* label, glm::vec4& vector, float speed, float minv, float maxv);

// create a file path field with file selector popup
bool OtUiFileSelector(const char* label, std::string& path);

// create a splitter widget
void OtUiSplitterVertical(float* size, float minSize, float maxSize);
void OtUiSplitterHorizontal(float* size, float minSize, float maxSize);

// selectors
bool OtUiSelectorEnum(const char* label, int& value, const char* const names[], size_t count);
bool OtUiSelectorPowerOfTwo(const char* label, int& value, int startValue, int endValue);
