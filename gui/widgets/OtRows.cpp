//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtArray.h"
#include "OtFunction.h"
#include "OtLog.h"

#include "OtUi.h"

#include "OtRows.h"


//
//	OtRowsClass::init
//

void OtRowsClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setResizable(parameters[2]->operator bool());
			[[fallthrough]];

		case 2:
			setBorders(parameters[1]->operator bool());
			[[fallthrough]];

		case 1:
			setHeights(parameters[0]);
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogError("[rows] constructor expects up to 3 arguments (not {})", count);
	}
}


//
//	OtRowsClass::setHeights
//

OtObject OtRowsClass::setHeights(OtObject heights) {
	// sanity check
	heights->expectKindOf("Array");

	// extract logical Row heights
	logicalHeights.clear();

	for (auto& height : OtArray(heights)->raw()) {
		float w = height->operator float();
		logicalHeights.push_back(w);
	}

	return OtWidget(this);
}


//
//	OtRowsClass::setBorders
//

OtObject OtRowsClass::setBorders(bool b) {
	borders = b;
	return OtWidget(this);
}


//
//	OtRowsClass::setResizable
//

OtObject OtRowsClass::setResizable(bool r) {
	resizable = r;
	return OtWidget(this);
}


//
//	OtRowsClass::render
//

void OtRowsClass::render() {
	// get available screen space
	auto available = ImGui::GetContentRegionAvail();

	// determine number of visible rows
	auto visibleRows = std::min(children.size(), logicalHeights.size());

	// determine total logical height of visible rows
	float totalLogicalHeight = 0.0f;

	for (size_t i = 0; i < visibleRows; i++) {
		totalLogicalHeight += logicalHeights[i];
	}

	// determine gap (splitter spacing)
	float gap = OtUi::getSplitterGap();

	// determine screen space for visible rows
	float space = visibleRows ? available.y - gap * (visibleRows - 1) : available.y;

	// determine ratio between logical and screen space
	float ratio = space / totalLogicalHeight;

	// determine screen height of each visible Row
	std::vector<float> screenHeights;

	for (size_t i = 0; i < visibleRows; i++) {
		screenHeights.push_back(logicalHeights[i] * ratio);
	}

	// render rows
	for (size_t i = 0; i < visibleRows; i++) {
		// ensure unique widget IDs
		ImGui::PushID(static_cast<int>(i));

		// render child widget
		auto spacing = ImGui::GetStyle().ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::BeginChild("row", ImVec2(0.0f, screenHeights[i]), borders ? ImGuiChildFlags_Borders : ImGuiChildFlags_None);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
		renderChild(children[i]);
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::PopStyleVar();

		// render separator (if required)
		if (i < visibleRows - 1) {
			if (resizable) {
				float newSize = screenHeights[i];

				if (OtUi::splitterVertical(&newSize, 0.0f, screenHeights[i] + screenHeights[i + 1])) {
					float delta = newSize - screenHeights[i];
					logicalHeights[i] = (screenHeights[i] + delta) / ratio;
					logicalHeights[i + 1] = (screenHeights[i + 1] - delta) / ratio;
				}

			} else {
				ImGui::Dummy(ImVec2(0.0f, gap));
			}
		}

		ImGui::PopID();
	}
}


//
//	OtRowsClass::getMeta
//

OtType OtRowsClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtRowsClass>("rows", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtRowsClass::init));
		type->set("setHeights", OtFunction::create(&OtRowsClass::setHeights));
		type->set("setBorders", OtFunction::create(&OtRowsClass::setBorders));
		type->set("setResizable", OtFunction::create(&OtRowsClass::setResizable));
	}

	return type;
}
