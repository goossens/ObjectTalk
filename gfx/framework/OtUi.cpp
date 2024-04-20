//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <string>

#include "glm/gtc/type_ptr.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

#include "OtNumbers.h"

#include "OtPathTools.h"
#include "OtUi.h"


//
//	OtUiIsMouseInRect
//

bool OtUiIsMouseInRect(const ImVec2& topLeft, const ImVec2& bottomRight) {
	ImVec2 mouse = ImGui::GetMousePos();
	return mouse.x >= topLeft.x && mouse.x <= bottomRight.x && mouse.y >= topLeft.y && mouse.y <= bottomRight.y;
}


//
//	OtUiSplitLabel
//

void OtUiSplitLabel(const char* text, std::string& label, std::string& id) {
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
	// get default width if required
	if (width <= 0.0f) {
		width = ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPos().x;
	}

	// get meta information
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
//	OtUiToggleButton
//

bool OtUiToggleButton(const char* labelPlusID, bool* value) {
	std::string label;
	std::string id;
	OtUiSplitLabel(labelPlusID, label, id);

	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	bool changed = false;

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.5f;

	ImGui::InvisibleButton(label.c_str(), ImVec2(width, height));

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
		ImVec2(p.x + radius + (*value ? 1 : 0) * (width - radius * 2.0f), p.y + radius),
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
//	OtUiInputText
//

bool OtUiInputText(const char* label, std::string* value, ImGuiInputTextFlags flags) {
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
	}, &value);
}


bool OtUiDragInt(const char* label, int* value, int minv, int maxv) {
	// automatically determine drag speed
	auto absValue = std::abs(*value);
	int speed;

	if (absValue < 100) {
		speed = 1;

	} else if (absValue < 1000) {
		speed = 10;

	} else {
		speed = 100;
	}

	if (ImGui::DragInt(label, value, speed, minv, maxv)) {
		*value = std::clamp(*value, minv, maxv);
		return true;

	} else {
		return false;
	}
}


//
//	OtUiDragFloat
//

bool OtUiDragFloat(const char* label, float* value, float minv, float maxv) {
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

	if (ImGui::DragFloat(label, value, speed, minv, maxv, format)) {
		*value = std::clamp(*value, minv, maxv);
		return true;

	} else {
		return false;
	}
}


//
//	OtUiEditVecX
//

static bool OtUiEditVecX(const char* labelPlusID, float* value, int components, float minv, float maxv) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	bool changed = false;

	std::string label;
	std::string id;
	OtUiSplitLabel(labelPlusID, label, id);

	ImGui::BeginGroup();
	ImGui::PushID(id.c_str());
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	static const ImU32 colors[] = { 0xBB0000FF, 0xBB00FF00, 0xBBFF0000, 0xBBFFFFFF };

	for (int i = 0; i < components; i++) {
		ImGui::PushID(i);
		changed |= OtUiDragFloat("##value", &value[i], minv, maxv);

		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();
		window->DrawList->AddLine(ImVec2(min.x, max.y - 1.0f), ImVec2(max.x, max.y - 1.0f), colors[i]);

		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
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
//	OtUiEditVec3
//

bool OtUiEditVec3(const char* label, glm::vec3* vector, float minv, float maxv) {
	return OtUiEditVecX(label, glm::value_ptr(*vector), 3, minv, maxv);
}


//
//	OtUiEditVec4
//

bool OtUiEditVec4(const char* label, glm::vec4* vector, float minv, float maxv) {
	return OtUiEditVecX(label, glm::value_ptr(*vector), 4, minv, maxv);
}


//
//	OtUiFileSelector
//

bool OtUiFileSelector(const char* label, std::string* path, const char* filter) {
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
	OtUiSplitLabel(label, realLabel, id);

	// get file dialog information
	auto dialog = ImGuiFileDialog::Instance();
	auto dialogID = std::string("select-file-") + id;

	// render widgets
	ImGui::PushID(id.c_str());
	bool changed = false;
	static bool creating = false;

	// get the filename without the path
	auto filename = OtPathGetFilename(*path);

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
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

	if (dialog->Display(dialogID.c_str(), ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		if (dialog->IsOk()) {
			*path = ImGuiFileDialog::Instance()->GetFilePathName();
			OtPathChangeDirectory(OtPathGetParent(*path));
			changed = true;
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
//	OtUiSelectorEnum
//

bool OtUiSelectorEnum(const char* label, int* value, const char* const names[], size_t count) {
	bool changed = false;

	if (ImGui::BeginCombo(label, names[*value])) {
		for (auto i = 0; i < count; i++) {
			if (ImGui::Selectable(names[i], i == *value)) {
				if (*value != i) {
					*value = i;
					changed = true;
				}
			}

			if (i == *value) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	return changed;
}


//
//	OtUiSelectorPowerOfTwo
//

bool OtUiSelectorPowerOfTwo(const char* label, int* value, int startValue, int endValue) {
	bool changed = false;

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
//	OtUiBezier
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

bool OtUiBezier(const char *label, float P[4]) {
	// based on https://github.com/ocornut/imgui/issues/786
	enum { SMOOTHNESS = 64 }; // curve smoothness: the higher number of segments, the smoother curve
	enum { CURVE_WIDTH = 3 }; // main curved line width
	enum { LINE_WIDTH = 1 }; // handlers: small lines width
	enum { GRAB_RADIUS = 6 }; // handlers: circle radius
	enum { GRAB_BORDER = 2 }; // handlers: circle border width

	static struct {
		const char *name;
		float points[4];
	} presets[] = {
		{ "Linear", 0.250f, 0.250f, 0.750f, 0.750f },
		{ "-", 0.0f, 0.0f, 0.0f, 0.0f },

		{ "In Sine", 0.470f, 0.000f, 0.745f, 0.715f },
		{ "In Quad", 0.550f, 0.085f, 0.680f, 0.530f },
		{ "In Cubic", 0.550f, 0.055f, 0.675f, 0.190f },
		{ "In Quart", 0.895f, 0.030f, 0.685f, 0.220f },
		{ "In Quint", 0.755f, 0.050f, 0.855f, 0.060f },
		{ "In Expo", 0.950f, 0.050f, 0.795f, 0.035f },
		{ "In Circ", 0.600f, 0.040f, 0.980f, 0.335f },
		{ "-", 0.0f, 0.0f, 0.0f, 0.0f },

		{ "Out Sine", 0.390f, 0.575f, 0.565f, 1.000f },
		{ "Out Quad", 0.250f, 0.460f, 0.450f, 0.940f },
		{ "Out Cubic", 0.215f, 0.610f, 0.355f, 1.000f },
		{ "Out Quart", 0.165f, 0.840f, 0.440f, 1.000f },
		{ "Out Quint", 0.230f, 1.000f, 0.320f, 1.000f },
		{ "Out Expo", 0.190f, 1.000f, 0.220f, 1.000f },
		{ "Out Circ", 0.075f, 0.820f, 0.165f, 1.000f },
		{ "-", 0.0f, 0.0f, 0.0f, 0.0f },

		{ "InOut Sine", 0.445f, 0.050f, 0.550f, 0.950f },
		{ "InOut Quad", 0.455f, 0.030f, 0.515f, 0.955f },
		{ "InOut Cubic", 0.645f, 0.045f, 0.355f, 1.000f },
		{ "InOut Quart", 0.770f, 0.000f, 0.175f, 1.000f },
		{ "InOut Quint", 0.860f, 0.000f, 0.070f, 1.000f },
		{ "InOut Expo", 1.000f, 0.000f, 0.000f, 1.000f },
		{ "InOut Circ", 0.785f, 0.135f, 0.150f, 0.860f },
	};

	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems) {
		return false;
	}

	// prepare canvas
	const float dim = ImGui::CalcItemWidth();
	ImVec2 canvas(dim, dim);
	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + canvas);
	ImGui::ItemSize(bb);
	ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

	// background grid
	for (int i = 0; i <= canvas.x; i += (canvas.x / 4)) {
		drawList->AddLine(
			ImVec2(bb.Min.x + i, bb.Min.y),
			ImVec2(bb.Min.x + i, bb.Max.y),
			ImGui::GetColorU32(ImGuiCol_TextDisabled));
	}

	for (int i = 0; i <= canvas.y; i += (canvas.y / 4)) {
		drawList->AddLine(
			ImVec2(bb.Min.x, bb.Min.y + i),
			ImVec2(bb.Max.x, bb.Min.y + i),
			ImGui::GetColorU32(ImGuiCol_TextDisabled));
	}

	// eval curve
	ImVec2 Q[4] = {{0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1}};
	ImVec2 results[SMOOTHNESS + 1];
	bezierTable<SMOOTHNESS>(Q, results);

	// handle grabbers
	bool changed = false;

	for (int i = 0; i < 2; i++) {
		float& px = P[i * 2 + 0];
		float& py = P[i * 2 + 1];

		ImVec2 pos = ImVec2(px, 1.0f - py) * (bb.Max - bb.Min) + bb.Min;
		ImGui::SetCursorScreenPos(pos - ImVec2(GRAB_RADIUS, GRAB_RADIUS));

		ImGui::PushID(i);
		ImGui::InvisibleButton("", ImVec2(2.0f * GRAB_RADIUS, 2.0f * GRAB_RADIUS));
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

	for (int i = 0; i < SMOOTHNESS; i++) {
		ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
		ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
		ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
		ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
		drawList->AddLine(r, s, color, CURVE_WIDTH);
	}

	// draw lines and grabbers
	float luma = ImGui::IsItemActive() || ImGui::IsItemHovered() ? 0.5f : 1.0f;
	ImVec4 pink(1.0f, 0.0f, 0.75f, luma), cyan(0.0f, 0.75f, 1.0f, luma);
	ImVec4 white(style.Colors[ImGuiCol_Text]);
	ImVec2 p1 = ImVec2(P[0], 1 - P[1]) * (bb.Max - bb.Min) + bb.Min;
	ImVec2 p2 = ImVec2(P[2], 1 - P[3]) * (bb.Max - bb.Min) + bb.Min;
	drawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH);
	drawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH);
	drawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
	drawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(pink));
	drawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
	drawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(cyan));

	// handle preset popup
	if (OtUiIsMouseInRect(bb.Min, bb.Max) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
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
	ImGui::SetCursorScreenPos(ImVec2(bb.Min.x, bb.Max.y + GRAB_RADIUS));

	return changed;
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
