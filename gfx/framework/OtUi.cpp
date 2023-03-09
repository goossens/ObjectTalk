//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "glm/ext.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

#include "OtUi.h"


//
//	OtUiCenteredText
//

void OtUiCenteredText(const char* text) {
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);
	ImGui::TextUnformatted(text);
}


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
//	OtUiReadonlyText
//

void OtUiReadonlyText(const char* label, std::string& value) {
	ImGui::InputText(label, (char*) value.c_str(), value.capacity() + 1, ImGuiInputTextFlags_ReadOnly);
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

bool OtUiFileSelector(
	const char* label, std::filesystem::path& path,
	std::function<void(std::filesystem::path& path)> create,
	std::function<void(std::filesystem::path& path)> edit) {

	// determine number of buttons
	int buttons =
		(path.empty() ? 0 : 1) +
		(path.empty() && create ? 1 : 0) +
		(!path.empty() && edit ? 1 : 0);

	// calculate component sizes
	ImVec2 pos = ImGui::GetCursorScreenPos();
	float itemWidth = ImGui::CalcItemWidth();
	float itemHeight = ImGui::GetFrameHeight();
	float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
	float buttonWidth = itemHeight + spacing;
	float pathWidth = itemWidth - buttonWidth * buttons;

	// get file dialog information
	auto dialog = ImGuiFileDialog::Instance();
	auto dialogID = std::string("select-file-") + label;

	// render widgets
	ImGui::PushID(label);
	bool changed = false;
	static bool creating = false;

	// render path as a button
	auto filename = path.filename().string();
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

	if (ImGui::Button((filename + "##path").c_str(), ImVec2(pathWidth, itemHeight))) {
		dialog->OpenDialog(
			dialogID.c_str(),
			"Select file...",
			".*",
			path.string(),
			1,
			nullptr,
			ImGuiFileDialogFlags_Modal |
				ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ReadOnlyFileNameField);
	}

	ImGui::PopStyleVar();

	// render clear button (if required)
	if (!path.empty()) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("x", ImVec2(itemHeight, itemHeight))) {
			path.clear();
			changed = true;
		}
	}

	// render create button (if required)
	if (path.empty() && create) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("+", ImVec2(itemHeight, itemHeight))) {
			auto directory = path.parent_path();

			if (directory.empty()) {
				directory = std::filesystem::current_path();
			}

			ImGuiFileDialog::Instance()->OpenDialog(
				dialogID.c_str(),
				"Create file...",
				".*",
				".",
				1,
				nullptr,
				ImGuiFileDialogFlags_Modal |
					ImGuiFileDialogFlags_DontShowHiddenFiles |
					ImGuiFileDialogFlags_ConfirmOverwrite);

			creating = true;
		}
	}

	// render edit button (if required)
	if (!path.empty() && edit) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("e", ImVec2(itemHeight, itemHeight))) {
			edit(path);
		}
	}

	// render label
	ImGui::SameLine(0.0f, spacing);
	ImGui::TextUnformatted(label);

	// show file selector (if required)
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

	if (dialog->Display(dialogID.c_str(), ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		if (dialog->IsOk()) {
			path = ImGuiFileDialog::Instance()->GetFilePathName();
			std::filesystem::current_path(std::filesystem::path(path).parent_path());
			changed = true;

			if (creating) {
				create(path);
			}
		}

		creating = false;
		dialog->Close();
	}

	ImGui::PopID();
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


//
//	OtUiLoadingIndicator
//

void OtUiLoadingIndicator(char* label, float radius, const ImVec4& color, const ImVec4& backdrop, int circles, float speed) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiContext& g = *GImGui;
	ImGuiID id = window->GetID(label);
	ImVec2 pos = window->DC.CursorPos;

	float circle = radius / 10.0f;
	ImRect bb(pos, ImVec2(pos.x + radius * 2.0f, pos.y + radius * 2.0f));
	ImGui::ItemSize(bb, g.Style.FramePadding.y);
	ImGui::ItemAdd(bb, id);

	float t = g.Time;
	auto offset = 2.0f * IM_PI / circles;

	for (int i = 0; i < circles; i++) {
		auto x = radius * std::sin(offset * i);
		auto y = radius * std::cos(offset * i);
		auto growth = std::max(0.0f, std::sin(t * speed - i * offset));

		ImVec4 color;
		color.x = color.x * growth + backdrop.x * (1.0f - growth);
		color.y = color.y * growth + backdrop.y * (1.0f - growth);
		color.z = color.z * growth + backdrop.z * (1.0f - growth);
		color.w = 1.0f;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + x, pos.y + radius - y), circle + growth * circle, ImGui::GetColorU32(color));
	}
}