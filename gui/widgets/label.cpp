//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "label.h"


//
//	OtLabelClass::init
//

void OtLabelClass::init(const std::string& l) {
	// save label
	label = l;
}


//
//	OtLabelClass::render
//

void OtLabelClass::render() {
	ImVec2 pos = ImGui::GetCursorPos();
	pos.y += 8;

	ImVec2 size = ImGui::CalcTextSize(label.c_str());
	size.y += 8;

	ImGui::InvisibleButton("##padded-text", size);
	ImVec2 nextPos = ImGui::GetCursorPos();

	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(label.c_str());
	ImGui::SetCursorPos(nextPos);
}


//
//	OtLabelClass::getMeta
//

OtType OtLabelClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtLabelClass>("Label", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtLabelClass::init));
	}

	return type;
}


//
//	OtLabelClass::create
//

OtLabel OtLabelClass::create() {
	OtLabel label = std::make_shared<OtLabelClass>();
	label->setType(getMeta());
	return label;
}
