//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtColumns.h"


//
//	OtColumnsClass::init
//

void OtColumnsClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setResizable(parameters[2]->operator bool());
			[[fallthrough]];

		case 2:
			setBorders(parameters[1]->operator bool());
			[[fallthrough]];

		case 1:
			setWidths(parameters[0]);
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogFatal("[Columns] constructor expects up to 3 arguments (not {})", count);
	}
}


//
//	OtColumnsClass::setWidths
//

OtObject OtColumnsClass::setWidths(OtObject widths) {
	// sanity check
	widths->expectKindOf("Array");

	// extract logical column widths
	logicalWidths.clear();

	for (auto& width : OtArray(widths)->raw()) {
		float w = width->operator float();
		logicalWidths.push_back(w);
	}

	return OtWidget(this);
}


//
//	OtColumnsClass::setBorders
//

OtObject OtColumnsClass::setBorders(bool b) {
	borders = b;
	return OtWidget(this);
}


//
//	OtColumnsClass::setResizable
//

OtObject OtColumnsClass::setResizable(bool r) {
	resizable = r;
	return OtWidget(this);
}


//
//	OtColumnsClass::render
//

void OtColumnsClass::render() {
	// get available screen space
	auto available = ImGui::GetContentRegionAvail();

	// determine number of visible columns
	auto visibleColumns = std::min(children.size(), logicalWidths.size());

	// determine total logical width of visible columns
	float totalLogicalWidth = 0.0f;

	for (size_t i = 0; i < visibleColumns; i++) {
		totalLogicalWidth += logicalWidths[i];
	}

	// determine gap (splitter spacing)
	float gap = OtUi::getSplitterGap();

	// determine screen space for visible columns
	float space = visibleColumns ? available.x - gap * (visibleColumns - 1) : available.x;

	// determine ratio between logical and screen space
	float ratio = space / totalLogicalWidth;

	// determine screen width of each visible column
	std::vector<float> screenWidths;

	for (size_t i = 0; i < visibleColumns; i++) {
		screenWidths.push_back(logicalWidths[i] * ratio);
	}

	// render columns
	for (size_t i = 0; i < visibleColumns; i++) {
		// ensure unique widget IDs
		ImGui::PushID(static_cast<int>(i));

		// render child widget
		ImGui::BeginChild("column", ImVec2(screenWidths[i], 0.0f), borders ? ImGuiChildFlags_Borders : ImGuiChildFlags_None);
		renderChild(children[i]);
		ImGui::EndChild();

		// render seperator (if required)
		if (i < visibleColumns - 1) {
			if (resizable) {
				float newSize = screenWidths[i];

				if (OtUi::splitterHorizontal(&newSize, 0.0f, screenWidths[i] + screenWidths[i + 1])) {
					float delta = newSize - screenWidths[i];
					logicalWidths[i] = (screenWidths[i] + delta) / ratio;
					logicalWidths[i + 1] = (screenWidths[i + 1] - delta) / ratio;
				}

			} else {
				ImGui::SameLine(0.0f, 0.0f);
				ImGui::Dummy(ImVec2(gap, 0.0f));
				ImGui::SameLine(0.0f, 0.0f);
			}
		}

		ImGui::PopID();
	}
}


//
//	OtColumnsClass::getMeta
//

OtType OtColumnsClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtColumnsClass>("Columns", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtColumnsClass::init));
		type->set("setWidths", OtFunction::create(&OtColumnsClass::setWidths));
		type->set("setBorders", OtFunction::create(&OtColumnsClass::setBorders));
		type->set("setResizable", OtFunction::create(&OtColumnsClass::setResizable));
	}

	return type;
}
