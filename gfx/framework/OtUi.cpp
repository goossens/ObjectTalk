//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#if _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

#include "fmt/format.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui_internal.h"
#include "imgui-knobs.h"
#include "ImGuiFileDialog.h"
#include "nlohmann/json.hpp"

#include "OtNumbers.h"
#include "OtPath.h"

#include "OtDejaVu.h"
#include "OtFontAudio.h"
#include "OtUi.h"


//
//	OtUi::loadFonts
//

void OtUi::loadFonts() {
	auto& io = ImGui::GetIO();
	io.Fonts->Clear();

	ImFontConfig textConfig;
	std::strncpy(textConfig.Name, "DejaVu", sizeof(textConfig.Name));
	textConfig.FontDataOwnedByAtlas = false;
	textConfig.OversampleH = 1;
	textConfig.OversampleV = 1;

	textFont = io.Fonts->AddFontFromMemoryTTF(
		reinterpret_cast<void*>(OtDejaVu::data()),
		static_cast<int>(OtDejaVu::size()),
		15.0f,
		&textConfig);

	ImFontConfig audioConfig;
	std::strncpy(audioConfig.Name, "FontAudio", sizeof(audioConfig.Name));
	audioConfig.FontDataOwnedByAtlas = false;
	audioConfig.OversampleH = 1;
	audioConfig.OversampleV = 1;

	audioFont = io.Fonts->AddFontFromMemoryTTF(
		reinterpret_cast<void*>(OtFontAudio::data()),
		static_cast<int>(OtFontAudio::size()),
		15.0f,
		&audioConfig);
}


//
//	OtUi::getAudioButtonWidth
//

float OtUi::getAudioButtonWidth() {
	return audioFont->CalcTextSizeA(15.0f, 100.0f, 100.0f, OtFontAudio::adr).x + ImGui::GetStyle().ItemInnerSpacing.x * 2.0f;
}


//
//	OtUi::align
//

void OtUi::align(ImVec2 size, Alignment horizontal, Alignment vertical) {
	// get current position and available space
	auto pos = ImGui::GetCursorScreenPos();
	auto available = ImGui::GetContentRegionAvail();

	// handle horizontal alignment (if required)
	if (horizontal == Alignment::center) {
		pos.x += (available.x - size.x) / 2.0f;

	} else if (horizontal == Alignment::right) {
		pos.x += available.x - size.x;
	}

	// handle horizontal alignment (if required)
	if (vertical == Alignment::middle) {
		pos.y += (available.y - size.y) / 2.0f;

	} else if (vertical == Alignment::bottom) {
		pos.y += available.y - size.y;
	}

	ImGui::SetCursorScreenPos(pos);
}


//
//	OtUi::isMouseInRect
//

bool OtUi::isMouseInRect(const ImVec2& topLeft, const ImVec2& bottomRight) {
	ImVec2 mouse = ImGui::GetMousePos();
	return mouse.x >= topLeft.x && mouse.x <= bottomRight.x && mouse.y >= topLeft.y && mouse.y <= bottomRight.y;
}


//
//	OtUi::splitLabel
//

void OtUi::splitLabel(const char* text, std::string& label, std::string& id) {
	std::string work(text);
	size_t pos = work.find("##");

	if (pos == std::string::npos) {
		label = work;
		id = work;

	} else {
		label = work.substr(0, pos);
		id = work.substr(pos + 2);
	}
}


//
//	OtUi::hSpacer
//

void OtUi::hSpacer(float width) {
	ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(width, 0.0f));
}


//
//	OtUi::centeredText
//

void OtUi::centeredText(const char* text) {
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);
	ImGui::TextUnformatted(text);
}


//
//	OtUi::header
//

void OtUi::header(const char* label, float width) {
	// get default width if required
	if (width <= 0.0f) {
		width = ImGui::GetContentRegionAvail().x;
	}

	// get information
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiID id = window->GetID(label);
	ImGuiStyle& style = ImGui::GetStyle();

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
		ImGui::GetColorU32(ImGuiCol_TabSelected));

	// render label
	ImGui::RenderTextClipped(
		bb.Min + style.FramePadding,
		bb.Max - style.FramePadding,
		label,
		nullptr,
		nullptr);
}


//
//	OtUi::toggleButton
//

bool OtUi::toggleButton(const char* labelPlusID, bool* value) {
	std::string label;
	std::string id;
	splitLabel(labelPlusID, label, id);

	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	auto changed = false;

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.5f;

	ImGui::InvisibleButton(id.c_str(), ImVec2(width, height));

	if (ImGui::IsItemClicked()) {
		*value = !*value;
		changed = true;
	}

	if (ImGui::IsItemHovered()) {
		drawlist->AddRectFilled(
			p,
			ImVec2(p.x + width, p.y + height),
			ImGui::GetColorU32(*value ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_ScrollbarGrabActive]),
			height * 0.5f);

	} else {
		drawlist->AddRectFilled(
			p,
			ImVec2(p.x + width, p.y + height),
			ImGui::GetColorU32(*value ? colors[ImGuiCol_Button] : colors[ImGuiCol_ScrollbarGrab]),
			height * 0.5f);
	}

	drawlist->AddCircleFilled(
		ImVec2(p.x + radius + (*value ? 1.0f : 0.0f) * (width - radius * 2.0f), p.y + radius),
		radius - 1.5f,
		IM_COL32(255, 255, 255, 255));

	// render label (if required)
	if (label.size()) {
		ImGui::SameLine(0.0f, ImGui::CalcItemWidth() - width + ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::TextUnformatted(label.c_str());
	}

	return changed;
}


//
//	OtUi::headerWithToggleButton
//

bool OtUi::headerWithToggleButton(const char* label, bool* value) {
	auto pos = ImGui::GetCursorScreenPos();
	auto regionWidth = ImGui::GetContentRegionAvail().x;
	header(label);

	float height = ImGui::GetFrameHeight();
	float width = height * 1.4f;
	pos.x += regionWidth - width;
	ImGui::SetCursorScreenPos(pos);

	ImVec4* colors = ImGui::GetStyle().Colors;
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	auto changed = false;

	auto id = fmt::format("##{}Button", label);
	ImGui::InvisibleButton(id.c_str(), ImVec2(width, height));

	if (ImGui::IsItemClicked()) {
		*value = !*value;
		changed = true;
	}

	float spacing = 3.0f;
	float diameter = height - spacing * 2.0f;
	float radius = diameter * 0.5f;
	pos += ImVec2(spacing, spacing);
	height = diameter;
	width = diameter * 1.55f;

	if (ImGui::IsItemHovered()) {
		drawlist->AddRectFilled(
			pos,
			ImVec2(pos.x + width, pos.y + height),
			ImGui::GetColorU32(*value ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_ScrollbarGrabActive]),
			radius);

	} else {
		drawlist->AddRectFilled(
			pos,
			ImVec2(pos.x + width, pos.y + height),
			ImGui::GetColorU32(*value ? colors[ImGuiCol_Button] : colors[ImGuiCol_ScrollbarGrab]),
			radius);
	}

	drawlist->AddCircleFilled(
		ImVec2(pos.x + radius + (*value ? 1.0f : 0.0f) * (width - radius * 2.0f), pos.y + radius),
		radius - 1.5f,
		IM_COL32(255, 255, 255, 255));

	return changed;
}


//
//	OtUi::latchButton
//

bool OtUi::latchButton(const char* label, bool* value, const ImVec2& size) {
	auto changed = false;
	ImVec4* colors = ImGui::GetStyle().Colors;

	if (*value) {
		ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_TableBorderLight]);

	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_TableBorderLight]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_TableBorderLight]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_ButtonActive]);
	}

	ImGui::Button(label, size);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
		*value = !*value;
		changed = true;
	}

	ImGui::PopStyleColor(3);
	return changed;
}


//
//	OtUi::readonlyText
//

void OtUi::readonlyText(const char* label, const std::string& value) {
	ImGui::InputText(label, (char*) value.c_str(), value.capacity() + 1, ImGuiInputTextFlags_ReadOnly);
}


//
//	OtUi::readonlyBool
//

void OtUi::readonlyBool(const char* label, bool value) {
	std::string text = value ? "true" : "false";
	readonlyText(label, text);
}


//
//	OtUi::readonlyInt
//

void OtUi::readonlyInt(const char* label, int value) {
	std::string text = std::to_string(value);
	readonlyText(label, text);
}


//
//	OtUi::readonlySizeT
//

void OtUi::readonlySizeT(const char* label, size_t value) {
	std::string text = std::to_string(value);
	readonlyText(label, text);
}


//
//	OtUi::readonlyFloat
//

void OtUi::readonlyFloat(const char* label, float value) {
	std::string text = std::to_string(value);
	readonlyText(label, text);
}


//
//	OtUi::inputText
//

bool OtUi::inputText(const char* label, std::string* value, ImGuiInputTextFlags flags) {
	inputString(label, value, flags);

	if (ImGui::IsItemDeactivatedAfterEdit()) {
		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			return false;

		} else {
			return true;
		}
	}

	return false;
}


//
//	OtUi::inputMultilineText
//

bool OtUi::inputMultilineText(const char* label, std::string* value, const ImVec2& size, ImGuiInputTextFlags flags) {
	flags |=
	ImGuiInputTextFlags_NoUndoRedo |
	ImGuiInputTextFlags_CallbackResize;

	ImGui::InputTextMultiline(label, (char*) value->c_str(), value->capacity() + 1, size, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = (std::string*) data->UserData;
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();
		}

		return 0;
	}, value);

	if (ImGui::IsItemDeactivatedAfterEdit()) {
		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			return false;

		} else {
			return true;
		}
	}

	return false;
}


//
//	OtUi::dragInt
//

bool OtUi::dragInt(const char* label, int* value, int minv, int maxv) {
	// automatically determine drag speed
	auto absValue = std::abs(*value);
	float speed;

	if (absValue < 100) {
		speed = 1.0f;

	} else if (absValue < 1000) {
		speed = 10.0f;

	} else {
		speed = 100.0f;
	}

	if (ImGui::DragInt(label, value, speed, minv, maxv, "%d", ImGuiSliderFlags_AlwaysClamp)) {
		*value = std::clamp(*value, minv, maxv);
		return true;

	} else {
		return false;
	}
}


//
//	OtUi::dragSizeT
//

bool OtUi::dragSizeT(const char* label, size_t* value, size_t minv, size_t maxv) {
	auto intValue = static_cast<int>(*value);
	auto intMinv = static_cast<int>(minv);
	auto intMaxv = static_cast<int>(maxv);
	auto result = dragInt(label, &intValue, intMinv, intMaxv);

	if (result) {
		*value = static_cast<size_t>(intValue);
	}

	return result;
}


//
//	OtUi::dragFloat
//

bool OtUi::dragFloat(const char* label, float* value, float minv, float maxv) {
	// automatically determine drag speed and display resolution
	auto absValue = std::abs(*value);
	float speed;
	const char* format;

	if (absValue < 1.0f) {
		speed = 0.01f;
		format = "%.3f";

	} else if (absValue < 10.0f) {
		speed = 0.1f;
		format = "%.2f";

	} else if (absValue < 100.0f) {
		speed = 1.0f;
		format = "%.1f";

	} else {
		speed = 10.0f;
		format = "%.0f";
	}

	if (ImGui::DragFloat(label, value, speed, minv, maxv, format, ImGuiSliderFlags_AlwaysClamp)) {
		*value = std::clamp(*value, minv, maxv);
		return true;

	} else {
		return false;
	}
}


//
//	OtUi::viewVecX
//

void OtUi::viewVecX(const char* labelPlusID, const float* value, int components) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	std::string label;
	std::string id;
	splitLabel(labelPlusID, label, id);

	ImGui::BeginGroup();
	ImGui::PushID(id.c_str());
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	static const ImU32 colors[] = { 0xBB0000FF, 0xBB00FF00, 0xBBFF0000, 0xBBFFFFFF };

	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);

		auto absValue = std::abs(value[i]);
		std::string text;

		if (absValue < 1.0f) {
			text = fmt::format("{:.3f}", value[i]);

		} else if (absValue < 10.0f) {
			text = fmt::format("{:.2f}", value[i]);

		} else if (absValue < 100.0f) {
			text = fmt::format("{:.1f}", value[i]);

		} else {
			text = fmt::format("{:.0f}", value[i]);
		}

		readonlyText("##value", text);

		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();
		window->DrawList->AddLine(ImVec2(min.x, max.y - 1.0f), ImVec2(max.x, max.y - 1.0f), colors[i]);

		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}

	ImGui::PopID();

	if (label.size()) {
		ImGui::TextUnformatted(label.c_str(), ImGui::FindRenderedTextEnd(label.c_str()));
	}

	ImGui::EndGroup();
}


//
//	OtUi::editVecX
//

bool OtUi::editVecX(const char* labelPlusID, float* value, int components, float minv, float maxv) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	auto changed = false;

	std::string label;
	std::string id;
	splitLabel(labelPlusID, label, id);

	ImGui::BeginGroup();
	ImGui::PushID(id.c_str());
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	static const ImU32 colors[] = { 0xBB0000FF, 0xBB00FF00, 0xBBFF0000, 0xBBFFFFFF };

	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);
		changed |= dragFloat("##value", &value[i], minv, maxv);

		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();
		window->DrawList->AddLine(ImVec2(min.x, max.y - 1.0f), ImVec2(max.x, max.y - 1.0f), colors[i]);

		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}

	ImGui::PopID();

	if (label.size()) {
		ImGui::TextUnformatted(label.c_str(), ImGui::FindRenderedTextEnd(label.c_str()));
	}

	ImGui::EndGroup();
	return changed;
}


//
//	OtUi::viewVec2
//

void OtUi::viewVec2(const char* label, const glm::vec2& vector) {
	viewVecX(label, glm::value_ptr(vector), 2);
}


//
//	OtUi::viewVec3
//

void OtUi::viewVec3(const char* label, const glm::vec3& vector) {
	viewVecX(label, glm::value_ptr(vector), 3);
}


//
//	OtUi::viewVec4
//

void OtUi::viewVec4(const char* label, const glm::vec4& vector) {
	viewVecX(label, glm::value_ptr(vector), 4);
}


//
//	OtUi::viewMat4
//

void OtUi::viewMat4(const char* labelPlusID, const glm::mat4& matrix) {
	auto m = glm::transpose(matrix);
	auto data = glm::value_ptr(m);

	std::string label;
	std::string id;
	splitLabel(labelPlusID, label, id);

	viewVecX(labelPlusID, data, 4);
	viewVecX(fmt::format("##{}1", label).c_str(), data + 4, 4);
	viewVecX(fmt::format("##{}2", label).c_str(), data + 8, 4);
	viewVecX(fmt::format("##{}3", label).c_str(), data + 12, 4);
}


//
//	OtUi::editVec2
//

bool OtUi::editVec2(const char* label, glm::vec2* vector, float minv, float maxv) {
	return editVecX(label, glm::value_ptr(*vector), 2, minv, maxv);
}


//
//	OtUi::editVec3
//

bool OtUi::editVec3(const char* label, glm::vec3* vector, float minv, float maxv) {
	return editVecX(label, glm::value_ptr(*vector), 3, minv, maxv);
}


//
//	OtUi::editVec4
//

bool OtUi::editVec4(const char* label, glm::vec4* vector, float minv, float maxv) {
	return editVecX(label, glm::value_ptr(*vector), 4, minv, maxv);
}


//
//	OtUi::fileSelector
//

bool OtUi::fileSelector(const char* label, std::string* path, const char* filter) {
	// determine button status
	bool showClearButton = !path->empty();

	// determine number of buttons
	int buttons = showClearButton ? 1 : 0;

	// calculate component sizes
	float itemWidth = ImGui::CalcItemWidth();
	float itemHeight = ImGui::GetFrameHeight();
	float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
	float buttonWidth = itemHeight + spacing;
	float pathWidth = itemWidth - buttonWidth * buttons;

	// determine real label and ID
	std::string realLabel;
	std::string id;
	splitLabel(label, realLabel, id);

	// get file dialog information
	auto dialog = ImGuiFileDialog::Instance();
	auto dialogID = std::string("select-file-") + id;

	// render widgets
	ImGui::PushID(id.c_str());
	auto changed = false;

	// get the filename without the path
	auto filename = OtPath::getFilename(*path);

	// render path as a button
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

	if (ImGui::Button((filename + "##path").c_str(), ImVec2(pathWidth, itemHeight))) {
		IGFD::FileDialogConfig config;
		config.filePathName = *path;
		config.countSelectionMax = 1;

		config.flags = ImGuiFileDialogFlags_Modal |
				ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ReadOnlyFileNameField;

		dialog->OpenDialog(dialogID.c_str(), "Select file...", filter, config);
	}

	ImGui::PopStyleVar();

	// render clear button (if required)
	if (showClearButton) {
		ImGui::SameLine(0.0f, spacing);

		if (ImGui::Button("x", ImVec2(itemHeight, itemHeight))) {
			path->clear();
			changed = true;
		}
	}

	// render label (if required)
	if (realLabel.size()) {
		ImGui::SameLine(0.0f, spacing);
		ImGui::TextUnformatted(realLabel.c_str());
	}

	// show file selector (if required)
	ImVec2 maxSize = ImGui::GetMainViewport()->Size;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

	if (dialog->Display(dialogID.c_str(), ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		if (dialog->IsOk()) {
			*path = ImGuiFileDialog::Instance()->GetFilePathName();
			OtPath::changeDirectory(OtPath::getParent(*path));
			changed = true;
		}

		dialog->Close();
	}

	ImGui::PopID();
	return changed;
}


//
//	OtUi::getSplitterGap
//

float OtUi::getSplitterGap() {
	return ImGui::GetStyle().ItemSpacing.y * 2.0f;
}


//
//	OtUi::splitter
//

bool OtUi::splitter(bool vertical, float* size, float minSize, float maxSize) {
	bool result = false;
	auto gap = OtUi::getSplitterGap();

	ImGui::PushID(size);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.4f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.3f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	if (!vertical) {
		ImGui::SameLine(0.0f, 0.0f);
	}

	ImGui::Button("##splitter", ImVec2(vertical ? -1.0f : gap, vertical ? gap : -1.0f));

	if (!vertical) {
		ImGui::SameLine(0.0f, 0.0f);
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	if (ImGui::IsItemActive()) {
		auto oldSize = *size;
		*size += vertical ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
		*size = std::clamp(*size, minSize, maxSize);

		if (*size != oldSize) {
			result = true;
		}
	}

	ImGui::PopID();
	return result;
}


//
//	OtUi::splitterVertical
//

bool OtUi::splitterVertical(float *size, float minSize, float maxSize) {
	return splitter(true, size, minSize, maxSize);
}


//
//	OtUi::splitterVertical
//

bool OtUi::splitterHorizontal(float* size, float minSize, float maxSize) {
	return splitter(false, size, minSize, maxSize);
}


//
//	OtUi::selectorPowerOfTwo
//

bool OtUi::selectorPowerOfTwo(const char* label, int* value, int startValue, int endValue) {
	auto changed = false;

	if (ImGui::BeginCombo(label, std::to_string(*value).c_str())) {
		for (auto size = startValue; size <= endValue; size <<= 1) {
			if (ImGui::Selectable(std::to_string(size).c_str(), size == *value)) {
				if (*value != size) {
					*value = size;
					changed = true;
				}
			}

			if (size == *value) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	return changed;
}


//
//	OtUi::bezier
//

template<int steps>
void bezierTable(ImVec2 P[4], ImVec2 results[steps + 1]) {
	static float C[(steps + 1) * 4];
	static bool initialized = false;

	if (!initialized) {
		for (unsigned step = 0; step <= steps; ++step) {
			float t = (float)step/(float)steps;
			C[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t);		// * P0
			C[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t;		// * P1
			C[step * 4 + 2] = 3 * (1 - t) * t * t;				// * P2
			C[step * 4 + 3] = t * t * t;						// * P3
		}

		initialized = true;
	}

	for (unsigned step = 0; step <= steps; ++step) {
		results[step] = {
			C[step * 4 + 0] * P[0].x + C[step * 4 + 1] * P[1].x + C[step * 4 + 2] * P[2].x + C[step * 4 + 3] * P[3].x,
			C[step * 4 + 0] * P[0].y + C[step * 4 + 1] * P[1].y + C[step * 4 + 2] * P[2].y + C[step * 4 + 3] * P[3].y};
	}
}

bool OtUi::bezier(const char* label, float P[4]) {
	// based on https://github.com/ocornut/imgui/issues/786
	static constexpr int smoothness = 64;	// curve smoothness: the higher number of segments, the smoother curve
	static constexpr int curveWidth = 3;	// main curved line width
	static constexpr int lineWidth = 1;		// handlers: small lines width
	static constexpr int grabRadius = 6;	// handlers: circle radius
	static constexpr int grabBorder = 2;	// handlers: circle border width

	static struct {
		const char* name;
		float points[4];
	} presets[] = {
		{"Linear", {0.250f, 0.250f, 0.750f, 0.750f}},
		{"-", {0.0f, 0.0f, 0.0f, 0.0f}},

		{"In Sine", {0.470f, 0.000f, 0.745f, 0.715f}},
		{"In Quad", {0.550f, 0.085f, 0.680f, 0.530f}},
		{"In Cubic", {0.550f, 0.055f, 0.675f, 0.190f}},
		{"In Quart", {0.895f, 0.030f, 0.685f, 0.220f}},
		{"In Quint", {0.755f, 0.050f, 0.855f, 0.060f}},
		{"In Expo", {0.950f, 0.050f, 0.795f, 0.035f}},
		{"In Circ", {0.600f, 0.040f, 0.980f, 0.335f}},
		{"-", {0.0f, 0.0f, 0.0f, 0.0f}},

		{"Out Sine", {0.390f, 0.575f, 0.565f, 1.000f}},
		{"Out Quad", {0.250f, 0.460f, 0.450f, 0.940f}},
		{"Out Cubic", {0.215f, 0.610f, 0.355f, 1.000f}},
		{"Out Quart", {0.165f, 0.840f, 0.440f, 1.000f}},
		{"Out Quint", {0.230f, 1.000f, 0.320f, 1.000f}},
		{"Out Expo", {0.190f, 1.000f, 0.220f, 1.000f}},
		{"Out Circ", {0.075f, 0.820f, 0.165f, 1.000f}},
		{"-", {0.0f, 0.0f, 0.0f, 0.0f}},

		{"InOut Sine", {0.445f, 0.050f, 0.550f, 0.950f}},
		{"InOut Quad", {0.455f, 0.030f, 0.515f, 0.955f}},
		{"InOut Cubic", {0.645f, 0.045f, 0.355f, 1.000f}},
		{"InOut Quart", {0.770f, 0.000f, 0.175f, 1.000f}},
		{"InOut Quint", {0.860f, 0.000f, 0.070f, 1.000f}},
		{"InOut Expo", {1.000f, 0.000f, 0.000f, 1.000f}},
		{"InOut Circ", {0.785f, 0.135f, 0.150f, 0.860f}},
	};

	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems) {
		return false;
	}

	ImGui::PushID(label);

	// prepare canvas
	const float dim = ImGui::CalcItemWidth();
	ImVec2 canvas(dim, dim);
	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + canvas);
	ImGui::ItemSize(bb);
	ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

	// background grid
	for (int i = 0; i <= static_cast<int>(canvas.x); i += static_cast<int>(canvas.x / 4)) {
		drawList->AddLine(
			ImVec2(bb.Min.x + i, bb.Min.y),
			ImVec2(bb.Min.x + i, bb.Max.y),
			ImGui::GetColorU32(ImGuiCol_TextDisabled));
	}

	for (int i = 0; i <= static_cast<int>(canvas.y); i += static_cast<int>(canvas.y / 4)) {
		drawList->AddLine(
			ImVec2(bb.Min.x, bb.Min.y + i),
			ImVec2(bb.Max.x, bb.Min.y + i),
			ImGui::GetColorU32(ImGuiCol_TextDisabled));
	}

	// eval curve
	ImVec2 Q[4] = {{0.0f, 0.0f}, {P[0], P[1]}, {P[2], P[3]}, {1.0f, 1.0f}};
	ImVec2 results[smoothness + 1];
	bezierTable<smoothness>(Q, results);

	// handle grabbers
	auto changed = false;

	for (int i = 0; i < 2; i++) {
		float& px = P[i * 2 + 0];
		float& py = P[i * 2 + 1];

		ImVec2 pos = ImVec2(px, 1.0f - py) * (bb.Max - bb.Min) + bb.Min;
		ImGui::SetCursorScreenPos(pos - ImVec2(grabRadius, grabRadius));

		ImGui::PushID(i);
		ImGui::InvisibleButton("", ImVec2(2.0f * grabRadius, 2.0f * grabRadius));
		ImGui::PopID();

		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			ImVec2 mouse = ImGui::GetMousePos() - bb.Min;
			float nx = mouse.x / dim;
			float ny = 1.0f - (mouse.y / dim);

			if (nx != px || ny != py) {
				px = std::clamp(nx, 0.0f, 1.0f);
				py = std::clamp(ny, 0.0f, 1.0f);
				changed = true;
			}
		}
	}

	// draw curve
	ImColor color(style.Colors[ImGuiCol_PlotLines]);

	for (int i = 0; i < smoothness; i++) {
		ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
		ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
		ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
		ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
		drawList->AddLine(r, s, color, curveWidth);
	}

	// draw lines and grabbers
	float luma = ImGui::IsItemActive() || ImGui::IsItemHovered() ? 0.5f : 1.0f;
	ImVec4 pink(1.0f, 0.0f, 0.75f, luma), cyan(0.0f, 0.75f, 1.0f, luma);
	ImVec4 white(style.Colors[ImGuiCol_Text]);
	ImVec2 p1 = ImVec2(P[0], 1 - P[1]) * (bb.Max - bb.Min) + bb.Min;
	ImVec2 p2 = ImVec2(P[2], 1 - P[3]) * (bb.Max - bb.Min) + bb.Min;
	drawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), lineWidth);
	drawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), lineWidth);
	drawList->AddCircleFilled(p1, grabRadius, ImColor(white));
	drawList->AddCircleFilled(p1, grabRadius - grabBorder, ImColor(pink));
	drawList->AddCircleFilled(p2, grabRadius, ImColor(white));
	drawList->AddCircleFilled(p2, grabRadius - grabBorder, ImColor(cyan));

	// handle preset popup
	if (isMouseInRect(bb.Min, bb.Max) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("presets");
	}

	if (ImGui::BeginPopup("presets")) {
		for (int i = 0; i < IM_ARRAYSIZE(presets); i++) {
			if (presets[i].name[0] == '-') {
				ImGui::Separator();

			} else {
				if (ImGui::MenuItem(presets[i].name)) {
					std::copy(presets[i].points, presets[i].points + 4, P);
				}
			}
		}

		ImGui::EndPopup();
	}

	// restore cursor pos
	ImGui::SetCursorScreenPos(ImVec2(bb.Min.x, bb.Max.y + grabRadius));
	ImGui::PopID();
	return changed;
}


//
//	OtUi::knob
//

bool OtUi::knob(const char* label, float* value, float minValue, float maxValue, const char* format, bool logarithmic) {
	static constexpr float pi = static_cast<float>(std::numbers::pi);

	return ImGuiKnobs::Knob(
		label, value, minValue, maxValue, 0.0f,
		format, ImGuiKnobVariant_WiperDot,
		0.0f, logarithmic ? ImGuiKnobFlags_Logarithmic : 0, 10,
		pi * 0.6f, pi * 2.4f);
}

bool OtUi::knob(const char* label, int* value, int minValue, int maxValue) {
	static constexpr float pi = static_cast<float>(std::numbers::pi);

	return ImGuiKnobs::KnobInt(
		label, value, minValue, maxValue, 0.0f,
		"%i", ImGuiKnobVariant_WiperDot,
		0.0f, 0, 10,
		pi * 0.6f, pi * 2.4f);
}


//
//	OtUi::knobWidth
//

float OtUi::knobWidth(size_t columns) {
	return (columns == 0) ? 0.0f : ImGui::GetTextLineHeight() * 4.0f * columns + ImGui::GetStyle().ItemSpacing.x * (columns - 1);
}


//
//	OtUi::knobHeight
//

float OtUi::knobHeight(size_t rows) {
	// title height
	auto height = ImGui::GetFrameHeightWithSpacing();

	// knob height
	height += ImGui::GetTextLineHeight() * 4.0f;

	// field height
	height += ImGui::GetFrameHeightWithSpacing();

	// return total height of knob widget
	return height * rows;
}


//
//	OtUi::trimSlider
//

bool OtUi::trimSlider(float* value) {
	ImGui::PushID(value);
	ImGui::SetNextItemWidth(ImGui::GetTextLineHeight() * 4.0f);
	auto result = ImGui::SliderFloat("", value, 0.0f, 1.0f, "%.2f");
	ImGui::PopID();
	return result;
}


//
//	OtUi::trimSliderWidth
//

float OtUi::trimSliderWidth() {
	return ImGui::GetTextLineHeight() * 4.0f;
}


//
//	OtUi::trimSliderHeight
//

float OtUi::trimSliderHeight() {
	return ImGui::GetTextLineHeight();
}


//
//	to/from_json
//

void to_json(nlohmann::json& j, const ImVec2& v) {
	j = nlohmann::json{v.x, v.y};
}

void from_json(const nlohmann::json& j, ImVec2& v) {
	v.x = j[0];
	v.y = j[1];
}
