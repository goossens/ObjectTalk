//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtFramework.h"
#include "OtMeter.h"


//
//	OtMeterClass::init
//

void OtMeterClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setLabel(parameters[2]->operator std::string());

		case 2:
			setMargin(parameters[1]->operator int());

		case 1:
			setTexture(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtExcept("[Meter] constructor expects up to 3 arguments (not %ld)", count);
	}
}


//
//	OtMeterClass::setTexture
//

OtObject OtMeterClass::setTexture(const std::string& textureName) {
	texture.loadFromFile(textureName);
	width = texture.getWidth();
	height = texture.getHeight();
	return OtObject(this);
}


//
//	OtMeterClass::setMargin
//

OtObject OtMeterClass::setMargin(int m) {
	margin = m;
	return OtObject(this);
}


//
//	OtMeterClass::setLabel
//

OtObject OtMeterClass::setLabel(const std::string& l) {
	label = l;
	return OtObject(this);
}


//
//	OtMeterClass::render
//

void OtMeterClass::render() {
	// sanity check
	if (!texture.isValid()) {
		OtExcept("No image provided for [Meter] widget");
	}

	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}

	// calculate image dimensions
	float steps = height / width;
	float h = height / steps;

	// calculate and set position
	float indent = ImGui::GetCursorPosX();
	float availableSpace = ImGui::GetWindowSize().x - indent;

	ImGui::SetCursorPos(ImVec2(
		indent + (availableSpace - width) / 2,
		ImGui::GetCursorPosY()));

	// render correct frame of image strip
	float offset1 = std::floor(value / 100.0 * (steps - 1)) * h;
	float offset2 = offset1 + h;

	ImGui::Image(
		(void*)(intptr_t) texture.getTextureIndex(),
		ImVec2(width, h),
		ImVec2(0, offset1 / height),
		ImVec2(1, offset2 / height));

		// render label if required
	if (label.size()) {
		ImGui::Dummy(ImVec2(0, 5));
		ImVec2 size = ImGui::CalcTextSize(label.c_str());
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(indent + (availableSpace - size.x) / 2, pos.y));
		ImGui::TextUnformatted(label.c_str());
	}

	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}
}


//
//	OtMeterClass::getMeta
//

OtType OtMeterClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMeterClass>("Meter", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtMeterClass::init));

		type->set("setTexture", OtFunction::create(&OtMeterClass::setTexture));
		type->set("setMargin", OtFunction::create(&OtMeterClass::setMargin));
		type->set("setLabel", OtFunction::create(&OtMeterClass::setLabel));

		type->set("setValue", OtFunction::create(&OtMeterClass::setValue));
		type->set("getValue", OtFunction::create(&OtMeterClass::getValue));
	}

	return type;
}
