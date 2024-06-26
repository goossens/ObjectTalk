//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtUi.h"

#include "OtLabel.h"


//
//	OtLabelClass::init
//

void OtLabelClass::init(size_t count, OtObject *parameters) {
	switch (count) {
		case 3:
			setVerticalAlignment(OtUiAlignment(parameters[2]->operator int()));

		case 2:
			setHorizontalAlignment(OtUiAlignment(parameters[1]->operator int()));

		case 1:
			setLabel(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[Label] constructor expects up to 3 arguments (not {})", count);
	}
}


//
//	OtLabelClass::render
//

void OtLabelClass::render() {
	// determine label size
	auto& style = ImGui::GetStyle();
	ImVec2 size = ImGui::CalcTextSize(label.c_str());
	size.y += style.FramePadding.y * 2.0f + style.ItemSpacing.y;

	// render label
	ImGui::Dummy(ImVec2(0.0f, style.FramePadding.y + style.ItemSpacing.y));
	OtUiAlign(size, horizontalAlign, verticalAlign);
	ImGui::TextUnformatted(label.c_str());
}


//
//	OtLabelClass::getMeta
//

OtType OtLabelClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtLabelClass>("Label", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtLabelClass::init));
		type->set("setLabel", OtFunction::create(&OtLabelClass::setLabel));
		type->set("setHorizontalAlignment", OtFunction::create(&OtLabelClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtLabelClass::setVerticalAlignment));
	}

	return type;
}
