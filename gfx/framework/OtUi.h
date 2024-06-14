//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <limits>
#include <string>

#include "glm/glm.hpp"
#include "imgui.h"
#include "nlohmann/json_fwd.hpp"


//
//	Constants
//

enum OtUiFont {
	uiDefaultFont = 0,
	uiEditorFont
};

enum OtUiAlignment {
	OtUiAlignNone,
	OtUiAlignLeft,
	OtUiAlignCenter,
	OtUiAlignRight,
	OtUiAlignTop,
	OtUiAlignMiddle,
	OtUiAlignBottom
};

#if __APPLE__
#define OT_UI_SHORTCUT "Cmd-"
#else
#define OT_UI_SHORTCUT "Ctrl-"
#endif


//
//	Functions
//

// determine psotional based on alignment
ImVec2 OtUiGetAlignedPosition(ImVec2 size, OtUiAlignment horizontal, OtUiAlignment vertical);

// is mouse in rectangle
bool OtUiIsMouseInRect(const ImVec2& topLeft, const ImVec2& bottomRight);

// split am ImGui label into a label and the ID
void OtUiSplitLabel(const char* text, std::string& label, std::string& id);

// render text centered in the current (child) window
void OtUiCenteredText(const char* text);

// create a header with specified width
void OtUiHeader(const char* label, float width=0.0f);

// create a toggle button
bool OtUiToggleButton(const char* label, bool* value);

// create a readonly text field
void OtUiReadonlyText(const char* label, std::string* value);

// create an input field based on a std::string
bool OtUiInputText(const char* label, std::string* value, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None);

// create a field to edit numbers
bool OtUiDragInt(const char* label, int* value, int minv=-std::numeric_limits<int>::max(), int maxv=std::numeric_limits<int>::max());
bool OtUiDragFloat(const char* label, float* value, float minv=-std::numeric_limits<float>::max(), float maxv=std::numeric_limits<float>::max());

// create a field to edit glm vectors
bool OtUiEditVec3(const char* label, glm::vec3* vector, float minv=-std::numeric_limits<float>::max(), float maxv=std::numeric_limits<float>::max());
bool OtUiEditVec4(const char* label, glm::vec4* vector, float minv=-std::numeric_limits<float>::max(), float maxv=std::numeric_limits<float>::max());

// create a file path field with file selector popup
bool OtUiFileSelector(const char* label, std::string* path, const char* filter);

// create a splitter widget
float OtUiGetSplitterGap();
bool OtUiSplitterVertical(float* size, float minSize, float maxSize);
bool OtUiSplitterHorizontal(float* size, float minSize, float maxSize);

// selectors
bool OtUiSelectorEnum(const char* label, int* value, const char* const names[], size_t count);
bool OtUiSelectorPowerOfTwo(const char* label, int* value, int startValue, int endValue);

// bezier curve editor
bool OtUiBezier(const char* label, float P[4]);


//
//	(De)serialization to/from JSON
//

void to_json(nlohmann::json& j, const ImVec2& v);
void from_json(const nlohmann::json& j, ImVec2& v);
