//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"
#include "imgui_internal.h"


//
//	OtUiInputText
//

inline bool OtUiInputText(const char* label, std::string& value) {
	ImGuiInputTextFlags flags =
		ImGuiInputTextFlags_NoUndoRedo |
		ImGuiInputTextFlags_CallbackResize;

	return ImGui::InputText(label, (char*) value.c_str(), value.capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = (std::string*) data->UserData;
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();
		}

		return 0;
	}, &value);
}


//
//	OtUiSplitter
//

inline void OtUiSplitter(bool vertical, float* size, float minSize, float maxSize) {
	auto thickness = ImGui::GetStyle().ItemSpacing.y * 2;

	ImGui::PushID(size);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5, 0.5, 0.5 ,0.4));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5, 0.5, 0.5 ,0.3));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	if (!vertical) {
		ImGui::SameLine();
	}

	ImGui::Button("##splitter", ImVec2(vertical ? -1 : thickness, vertical ? thickness : -1));

	if (!vertical) {
		ImGui::SameLine();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	if (ImGui::IsItemActive()) {
		*size += vertical ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
		*size = std::clamp(*size, minSize, maxSize);
	}

	ImGui::PopID();
}

inline void OtUiSplitterVertical(float* size, float minSize, float maxSize) {
	OtUiSplitter(true, size, minSize, maxSize);
}

inline void OtUiSplitterHorizontal(float* size, float minSize, float maxSize) {
	OtUiSplitter(false, size, minSize, maxSize);
}


//
//	OtUiEditVecX
//

inline bool OtUiEditVecX(const char* label, float* v, int components, float speed, float minv, float maxv, const char* format) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems) {
		return false;
	}

	ImGuiContext& g = *GImGui;
	bool changed = false;

	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	for (int i = 0; i < components; i++) {
		static const ImU32 colors[] = { 0xBB0000FF, 0xBB00FF00, 0xBBFF0000, 0xBBFFFFFF };

		ImGui::PushID(i);
		changed |= ImGui::DragFloat("##v", &v[i], speed, minv, maxv, format);

		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();
		window->DrawList->AddLine(ImVec2(min.x, max.y - 1), ImVec2(max.x, max.y - 1), colors[i], 1);

		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}

	ImGui::PopID();
	ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
	ImGui::EndGroup();

	return changed;
}


//
//	OtUiEditVec3
//

static inline bool OtUiEditVec3(const char* label, glm::vec3& vector, float speed, float minv, float maxv, const char* format) {
	return OtUiEditVecX(label, glm::value_ptr(vector), 3, speed, minv, maxv, format);
}


//
//	OtUiEditVec4
//

static inline bool OtUiEditVec4(const char* label, glm::vec4& vector, float speed, float minv, float maxv, const char* format) {
	return OtUiEditVecX(label, glm::value_ptr(vector), 4, speed, minv, maxv, format);
}
