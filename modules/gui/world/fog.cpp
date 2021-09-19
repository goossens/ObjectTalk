//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "glm/ext.hpp"

#include "ot/function.h"

#include "color.h"
#include "fog.h"


//
//	OtFogClass::setColor
//

OtObject OtFogClass::setColor(const std::string& c) {
	color = OtColorParseToVec4(c);
	return shared();
}


//
//	OtFogClass::setDistances
//

OtObject OtFogClass::setDistances(double n, double f) {
	near = n;
	far = f;
	return shared();
}


//
//	OtFogClass::renderGUI
//

void OtFogClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::ColorEdit4("Color", glm::value_ptr(color));
	ImGui::SliderFloat("Near", &near, 0.0f, 100.0f);
	ImGui::SliderFloat("Far", &far, 0.0f, 100.0f);
}


//
//	OtFogClass::submit
//

void OtFogClass::submit(glm::vec4* slot) {
	// pass fog information
	slot[0] = { enabled, near, far, 0.0 };
	slot[1] = color;
}


//
//	OtFogClass::getMeta
//

OtType OtFogClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtFogClass>("Fog", OtSceneObjectClass::getMeta());
		type->set("setColor", OtFunctionClass::create(&OtFogClass::setColor));
		type->set("setDistances", OtFunctionClass::create(&OtFogClass::setDistances));
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
