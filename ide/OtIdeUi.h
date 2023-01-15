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

#include "imgui.h"


//
//	OtIdeUiSplitter
//

inline void OtIdeUiSplitter(bool vertical, float* size, float minSize, float maxSize) {
	ImVec2 pos = ImGui::GetCursorPos();

	if (vertical) {
		ImGui::SetCursorPosY(pos.y + *size);

	} else {
		ImGui::SetCursorPosX(pos.x + *size);
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5, 0.5, 0.5 ,0.2));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5, 0.5, 0.5 ,0.2));
	auto thickness = (vertical ? ImGui::GetStyle().ItemSpacing.y : ImGui::GetStyle().ItemSpacing.y * 2.0);
	ImGui::Button("##splitter", ImVec2(!vertical ? thickness : -1.0, vertical ? thickness : -1.0));
	ImGui::PopStyleColor(3);

	ImGui::SetItemAllowOverlap();

	if (ImGui::IsItemActive()) {
		*size += vertical ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
		*size = std::clamp(*size, minSize, maxSize);
	}

	ImGui::SetCursorPos(pos);
}

inline void OtIdeUiSplitterVertical(float* size, float minSize, float maxSize) {
	OtIdeUiSplitter(true, size, minSize, maxSize);
}

inline void OtIdeUiSplitterHorizontal(float* size, float minSize, float maxSize) {
	OtIdeUiSplitter(false, size, minSize, maxSize);
}
