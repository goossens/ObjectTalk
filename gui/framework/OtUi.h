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
#include "imgui_internal.h"


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
//	OtUiDragFloat
//

inline bool OtUiDragFloat(const char* label, float* v, int components, float minv, float maxv, float speed, const char* format) {
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
		const float spacing = g.Style.FrameRounding;
		const float halfSpacing = spacing / 2;

		window->DrawList->AddLine({ min.x + spacing, max.y - halfSpacing }, { max.x - spacing, max.y - halfSpacing }, colors[i], 2);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

		ImGui::PopID();
		ImGui::PopItemWidth();
	}

	ImGui::PopID();
	ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
	ImGui::EndGroup();

	return changed;
}