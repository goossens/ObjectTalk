//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "glm/ext.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

#include "OtUi.h"


//
//	OtUiHeader
//

void OtUiHeader(const char* label, float width) {
	// get meta information
    ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiID id = window->GetID(label);
	ImGuiStyle& style = g.Style;

	// calculate size of header
	ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);
    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, labelSize.y + style.FramePadding.y * 2.0f));

	//reserve space
	ImGui::ItemSize(bb, style.FramePadding.y);
	ImGui::ItemAdd(bb, id, &bb);

	// render background
	window->DrawList->AddRectFilled(
		bb.Min,
		bb.Max,
		ImGui::GetColorU32(ImGuiCol_Header),
		style.FrameRounding);

	window->DrawList->AddLine(
		ImVec2(bb.Min.x, bb.Max.y - 1.0f),
		ImVec2(bb.Max.x, bb.Max.y - 1.0f),
		ImGui::GetColorU32(ImGuiCol_TabActive));

	// render label
	ImGui::RenderTextClipped(
		bb.Min + style.FramePadding,
		bb.Max - style.FramePadding,
		label,
		nullptr,
		nullptr);
}


//
//	OtUiInputText
//

bool OtUiInputText(const char* label, std::string& value) {
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
//	OtUiEditVecX
//

static bool OtUiEditVecX(const char* label, float* v, int components, float speed, float minv, float maxv) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems) {
		return false;
	}

	ImGuiContext& g = *GImGui;
	bool changed = false;

	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	static const ImU32 colors[] = { 0xBB0000FF, 0xBB00FF00, 0xBBFF0000, 0xBBFFFFFF };
	static const char* format[] = { "X:%.1f", "Y:%.1f", "Z:%.1f", "W:%.1f" };

	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);
		changed |= ImGui::DragFloat("##v", &v[i], speed, minv, maxv, format[i]);

		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();
		window->DrawList->AddLine(ImVec2(min.x, max.y - 1.0f), ImVec2(max.x, max.y - 1.0f), colors[i]);

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

bool OtUiEditVec3(const char* label, glm::vec3& vector, float speed, float minv, float maxv) {
	return OtUiEditVecX(label, glm::value_ptr(vector), 3, speed, minv, maxv);
}


//
//	OtUiEditVec4
//

bool OtUiEditVec4(const char* label, glm::vec4& vector, float speed, float minv, float maxv) {
	return OtUiEditVecX(label, glm::value_ptr(vector), 4, speed, minv, maxv);
}


//
//	OtUiFileSelector
//

bool OtUiFileSelector(const char* label, std::filesystem::path& path) {
	// get meta information
    ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiID id = window->GetID(label);
	ImGuiStyle& style = g.Style;

	// calculate size of field
	ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);
	float itemWidth = ImGui::CalcItemWidth();
    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(itemWidth, labelSize.y + style.FramePadding.y * 2.0f));
    ImRect totalBB(bb.Min, bb.Max + ImVec2(labelSize.x > 0.0f ? style.ItemInnerSpacing.x + labelSize.x : 0.0f, 0.0f));

	ImGui::ItemSize(totalBB, style.FramePadding.y);
	ImGui::ItemAdd(totalBB, id, &bb);

	bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	// render background
	window->DrawList->AddRectFilled(
		bb.Min,
		bb.Max,
		ImGui::GetColorU32((hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button),
		style.FrameRounding,
		ImDrawFlags_RoundCornersAll);

	ImGui::RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);

	// render path
	auto filename = path.filename().string();

	ImGui::RenderTextClipped(
		bb.Min + style.FramePadding,
		bb.Max - style.FramePadding,
		filename.c_str(),
		nullptr,
		nullptr);

	// render label
	if (labelSize.x > 0) {
		ImGui::RenderText(ImVec2(bb.Max.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), label);
	}

	// show file selector if required
	auto dialog = ImGuiFileDialog::Instance();
	auto pathString = path.string();
	auto dialogID = std::string("select-file-") + label;

	if (pressed && !dialog->IsOpened()) {
		dialog->OpenDialog(
			dialogID.c_str(),
			"Select file...",
			".*",
			pathString,
			1,
			nullptr,
			ImGuiFileDialogFlags_Modal |
				ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ReadOnlyFileNameField);
	}

	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);
	bool changed = false;

	if (dialog->Display(dialogID.c_str(), ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		if (dialog->IsOk()) {
			auto selected = dialog->GetSelection();
			path = selected.begin()->second;
			changed = true;
		}

		dialog->Close();
	}

	return changed;
}


//
//	OtUiSplitter
//

static void OtUiSplitter(bool vertical, float* size, float minSize, float maxSize) {
	auto thickness = ImGui::GetStyle().ItemSpacing.y * 2.0f;

	ImGui::PushID(size);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.4f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.3f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	if (!vertical) {
		ImGui::SameLine();
	}

	ImGui::Button("##splitter", ImVec2(vertical ? -1.0f : thickness, vertical ? thickness : -1.0f));

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

void OtUiSplitterVertical(float* size, float minSize, float maxSize) {
	OtUiSplitter(true, size, minSize, maxSize);
}

void OtUiSplitterHorizontal(float* size, float minSize, float maxSize) {
	OtUiSplitter(false, size, minSize, maxSize);
}
