//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#if __APPLE__
#define OT_UI_SHORTCUT "Cmd-"
#else
#define OT_UI_SHORTCUT "Ctrl-"
#endif


//
//	OtUi
//

class OtUi {
public:
	enum class Font {
		standard = 0,
		editor
	};

	enum class Alignment {
		none,
		left,
		center,
		right,
		top,
		middle,
		bottom
	};

	// adjust cursor position based on alignment
	static void align(ImVec2 size, Alignment horizontal, Alignment vertical);

	// is mouse in rectangle?
	static bool isMouseInRect(const ImVec2& topLeft, const ImVec2& bottomRight);

	// split a Dear ImGui label into a label and the ID
	static void splitLabel(const char* text, std::string& label, std::string& id);

	// add horizontal spacing
	static void hSpacer(float width);

	// render text centered in the current (child) window
	static void centeredText(const char* text);

	// header with specified width
	static void header(const char* label, float width=0.0f);

	// toggle button
	static bool toggleButton(const char* label, bool* value);

	// latch button
	static bool latchButton(const char* label, bool* value, const ImVec2& size=ImVec2(0.0f, 0.0f));

	// radio button
	template <typename T>
	static bool radioButton(const char* label, T* value, T buttonValue, const ImVec2& size=ImVec2(0.0f, 0.0f))  {
		bool changed = false;
		ImVec4* colors = ImGui::GetStyle().Colors;

		if (*value == buttonValue) {
			ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_ButtonActive]);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_TableBorderLight]);

		} else {
			ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_TableBorderLight]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_TableBorderLight]);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_ButtonActive]);
		}

		ImGui::Button(label, size);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && *value != buttonValue) {
			*value = buttonValue;
			changed = true;
		}

		ImGui::PopStyleColor(3);
		return changed;
	}

	// readonly fields
	static void readonlyText(const char* label, std::string* value);
	static void readonlyInt(const char* label, int value);
	static void readonlySizeT(const char* label, size_t value);
	static void readonlyFloat(const char* label, float value);

	// field based on a std::string
	static inline bool inputString(const char* label, std::string* value, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None) {
		flags |=
			ImGuiInputTextFlags_NoUndoRedo |
			ImGuiInputTextFlags_CallbackResize;

		return ImGui::InputText(label, (char*) value->c_str(), value->capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
				std::string* value = (std::string*) data->UserData;
				value->resize(data->BufTextLen);
				data->Buf = (char*) value->c_str();
			}

			return 0;
		}, value);
	}

	static bool inputText(const char* label, std::string* value, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None);
	static bool inputMultilineText(const char* label, std::string* value, const ImVec2& size, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None);

	// field to edit numbers
	static bool dragInt(const char* label, int* value, int minv=std::numeric_limits<int>::lowest(), int maxv=std::numeric_limits<int>::max());
	static bool dragSizeT(const char* label, size_t* value, size_t minv=0, size_t maxv=std::numeric_limits<size_t>::max());
	static bool dragFloat(const char* label, float* value, float minv=std::numeric_limits<float>::lowest(), float maxv=std::numeric_limits<float>::max());

	// field to edit glm vectors
	static bool editVec2(const char* label, glm::vec2* vector, float minv=std::numeric_limits<float>::lowest(), float maxv=std::numeric_limits<float>::max());
	static bool editVec3(const char* label, glm::vec3* vector, float minv=std::numeric_limits<float>::lowest(), float maxv=std::numeric_limits<float>::max());
	static bool editVec4(const char* label, glm::vec4* vector, float minv=std::numeric_limits<float>::lowest(), float maxv=std::numeric_limits<float>::max());

	// file path field with file selector popup
	static bool fileSelector(const char* label, std::string* path, const char* filter);

	// splitter widgets
	static float getSplitterGap();
	static bool splitterVertical(float* size, float minSize, float maxSize);
	static bool splitterHorizontal(float* size, float minSize, float maxSize);

	// selectors
	template <typename T>
	static bool selectorEnum(const char* label, T* value, const char* const names[], size_t count) {
		bool changed = false;

		if (ImGui::BeginCombo(label, names[static_cast<size_t>(*value)])) {
			for (size_t i = 0; i < count; i++) {
				if (ImGui::Selectable(names[i], static_cast<size_t>(*value) == i)) {
					if (static_cast<size_t>(*value) != i) {
						*value = static_cast<T>(i);
						changed = true;
					}
				}

				if (i == static_cast<size_t>(*value)) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		return changed;
	}

	static bool selectorPowerOfTwo(const char* label, int* value, int startValue, int endValue);

	// bezier curve editor
	static bool bezier(const char* label, float P[4]);

private:
	// generic vector editor
	static bool editVecX(const char* labelPlusID, float* value, int components, float minv, float maxv);

	// generic splitter
	static bool splitter(bool vertical, float* size, float minSize, float maxSize);
};


//
//	(De)serialization to/from JSON
//

void to_json(nlohmann::json& j, const ImVec2& v);
void from_json(const nlohmann::json& j, ImVec2& v);
