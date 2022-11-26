//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "bimg/bimg.h"

#include "imgui.h"

#include "OtCallback.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtVM.h"

#include "OtFramework.h"
#include "OtKnob.h"


//
//	OtKnobClass::init
//
OtObject callback;

OtObject OtKnobClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 4:
			setCallback(parameters[3]);

		case 3:
			setLabel(parameters[2]->operator std::string());

		case 2:
			setMargin(parameters[1]->operator int());

		case 1:
			setTexture(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtExcept("[Knob] constructor expects up to 4 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtKnobClass::setTexture
//

OtObject OtKnobClass::setTexture(const std::string& textureName) {
	if (bgfx::isValid(texture)) {
		OtExcept("Texture already set for [Knob] widget");
	}

	bimg::ImageContainer* image;
	texture = OtFrameworkClass::instance()->getTexture(textureName, false, &image);
	width = image->m_width;
	height = image->m_height;
	return shared();
}


//
//	OtKnobClass::setMargin
//

OtObject OtKnobClass::setMargin(int m) {
	margin = m;
	return shared();
}


//
//	OtKnobClass::setLabel
//

OtObject OtKnobClass::setLabel(const std::string& l) {
	label = l;
	return shared();
}


//
//	OtKnobClass::setCallback
//

OtObject OtKnobClass::setCallback(OtObject cb) {
	OtCallbackValidate(cb, 1);
	callback = cb;
	return shared();
}


//
//	OtKnobClass::render
//

void OtKnobClass::render() {
	// sanity check
	if (!bgfx::isValid(texture)) {
		OtExcept("No image provided for [Knob] widget");
	}

	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}

	// calculate image dimensions
	float steps = height / width;
	float h = height / steps;

	// calculate position
	float indent = ImGui::GetCursorPosX();
	float availableSpace = ImGui::GetWindowSize().x - indent;
	ImVec2 pos = ImVec2(indent + (availableSpace - width) / 2, ImGui::GetCursorPosY());

	// setup new widget
	ImGui::PushID((void*) this);
	ImGui::SetCursorPos(pos);
	ImGui::InvisibleButton("", ImVec2(width, h), 0);
	ImGuiIO& io = ImGui::GetIO();

	// detect mouse activity
	if (ImGui::IsItemActive() && io.MouseDelta.y != 0.0) {
		auto newValue = std::clamp(value - io.MouseDelta.y / 2.0, 0.0, 100.0);

		// call user callback if value has changed
		if (callback && newValue != value) {
			OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(value));
		}

		value = newValue;
	}

	ImGui::PopID();

	// render correct frame of image strip
	ImGui::SetCursorPos(pos);

	float offset1 = std::floor(value / 100.0 * (steps - 1)) * h;
	float offset2 = offset1 + h;

	ImGui::Image(
		(void*)(intptr_t) texture.idx,
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
//	OtKnobClass::getMeta
//

OtType OtKnobClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtKnobClass>("Knob", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtKnobClass::init));

		type->set("setTexture", OtFunctionClass::create(&OtKnobClass::setTexture));
		type->set("setMargin", OtFunctionClass::create(&OtKnobClass::setMargin));
		type->set("setLabel", OtFunctionClass::create(&OtKnobClass::setLabel));
		type->set("setCallback", OtFunctionClass::create(&OtKnobClass::setCallback));

		type->set("setValue", OtFunctionClass::create(&OtKnobClass::setValue));
		type->set("getValue", OtFunctionClass::create(&OtKnobClass::getValue));
	}

	return type;
}


//
//	OtKnobClass::create
//

OtKnob OtKnobClass::create() {
	OtKnob knob = std::make_shared<OtKnobClass>();
	knob->setType(getMeta());
	return knob;
}
