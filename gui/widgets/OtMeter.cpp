//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtMeter.h"


//
//	OtMeterClass::init
//

OtObject OtMeterClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setLabel(parameters[2]->operator std::string());

		case 2:
			setMargin(parameters[1]->operator int());

		case 1:
			setTexture(parameters[0]);

		case 0:
			break;

		default:
			OtExcept("[Meter] constructor expects up to 3 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtMeterClass::setTexture
//
OtObject OtMeterClass::setTexture(OtObject object) {
	// a texture can be a texture object or the name of an inamge file
	if (object->isKindOf("Texture")) {
		texture = object->cast<OtTextureClass>();

	} else if (object->isKindOf("String")) {
		texture = OtTextureClass::create();
		texture->loadImage(object->operator std::string());

	} else {
		OtExcept("Expected a [Texture] or [String] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtMeterClass::setMargin
//

OtObject OtMeterClass::setMargin(int m) {
	margin = m;
	return shared();
}


//
//	OtMeterClass::setLabel
//

OtObject OtMeterClass::setLabel(const std::string& l) {
	label = l;
	return shared();
}


//
//	OtMeterClass::render
//

void OtMeterClass::render() {
	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}

	if (texture) {
		// calculate image dimensions
		float width = texture->getWidth();
		float fullHeight = texture->getHeight();
		float steps = fullHeight / width;
		float height = fullHeight / steps;

		// calculate and set position
		float indent = ImGui::GetCursorPosX();
		float availableSpace = ImGui::GetWindowSize().x - indent;

		ImGui::SetCursorPos(ImVec2(
			indent + (availableSpace - width) / 2,
			ImGui::GetCursorPosY()));

			// render correct frame of image strip
		float offset1 = std::floor(value / 100.0 * (steps - 1)) * height;
		float offset2 = offset1 + height;

		ImGui::Image(
			(void*)(intptr_t) texture->getTextureHandle().idx,
			ImVec2(width, height),
			ImVec2(0, offset1 / fullHeight),
			ImVec2(1, offset2 / fullHeight));

			// render label if required
		if (label.size()) {
			ImGui::Dummy(ImVec2(0, 5));
			ImVec2 size = ImGui::CalcTextSize(label.c_str());
			ImVec2 pos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(indent + (availableSpace - size.x) / 2, pos.y));
			ImGui::TextUnformatted(label.c_str());
		}
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
		type->set("__init__", OtFunctionClass::create(&OtMeterClass::init));

		type->set("setTexture", OtFunctionClass::create(&OtMeterClass::setTexture));
		type->set("setMargin", OtFunctionClass::create(&OtMeterClass::setMargin));
		type->set("setLabel", OtFunctionClass::create(&OtMeterClass::setLabel));

		type->set("setValue", OtFunctionClass::create(&OtMeterClass::setValue));
		type->set("getValue", OtFunctionClass::create(&OtMeterClass::getValue));
	}

	return type;
}


//
//	OtMeterClass::create
//

OtMeter OtMeterClass::create() {
	OtMeter meter = std::make_shared<OtMeterClass>();
	meter->setType(getMeta());
	return meter;
}
