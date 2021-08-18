//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <imgui.h>

#include "ot/function.h"

#include "color.h"
#include "fog.h"


//
//	OtFogClass::OtFogClass
//

OtFogClass::OtFogClass() {
	// register uniform
	fogUniform = bgfx::createUniform("u_fog", bgfx::UniformType::Vec4, 2);
}


//
//	OtFogClass::~OtFogClass
//

OtFogClass::~OtFogClass() {
	// release resources
	if (fogUniform.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(fogUniform);
	}
}


//
//	OtFogClass::init
//

OtObject OtFogClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 4:
				far = parameters[3]->operator double();

			case 3:
				near = parameters[2]->operator double();

			case 2:
				color = OtColorParseToVec4(parameters[1]->operator std::string());

			case 1:
				active = parameters[0]->operator bool();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Fog] contructor (max 4)", count);
		}
	}

	return nullptr;
}


//
//	OtFogClass::renderGUI
//

void OtFogClass::renderGUI() {
	int flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx(this, flags, "Fog:")) {
		ImGui::ColorEdit4("Color", glm::value_ptr(color));
		ImGui::SliderFloat("Near", &near, 0.0f, 100.0f);
		ImGui::SliderFloat("Far", &far, 0.0f, 100.0f);
		ImGui::Checkbox("active", &active);
		ImGui::TreePop();
	}
}


//
//	OtFogClass::submit
//

void OtFogClass::submit() {
	// pass fog information
	glm::vec4 fogUniforms[2];
	fogUniforms[0] = { active, near, far, 0.0 };
	fogUniforms[1] = color;
	bgfx::setUniform(fogUniform, &fogUniforms, 2);
}


//
//	OtFogClass::getMeta
//

OtType OtFogClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtFogClass>("Fog", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtFogClass::init));
	}

	return type;
}


//
//	OtFogClass::create
//

OtFog OtFogClass::create() {
	OtFog fog = std::make_shared<OtFogClass>();
	fog->setType(getMeta());
	return fog;
}
