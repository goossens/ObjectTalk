//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "ot/function.h"

#include "color.h"
#include "light.h"


//
//	OtLightClass::setPosition
//

OtObject OtLightClass::setPosition(double px, double py, double pz) {
	position = glm::vec4(px, py, pz, 1.0);
	return shared();
}


//
//	OtLightClass::setDiffuse
//

OtObject OtLightClass::setDiffuse(const std::string c) {
	diffuse = OtColorParseToVec4(c);
	return shared();
}


//
//	OtLightClass::setSpecular
//

OtObject OtLightClass::setSpecular(const std::string c) {
	specular = OtColorParseToVec4(c);
	return shared();
}


//
//	OtLightClass::renderGUI
//

void OtLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat3("Position", glm::value_ptr(position), -50.0f, 50.0f);
	ImGui::ColorEdit4("Diffuse", glm::value_ptr(diffuse));
	ImGui::ColorEdit4("Specular", glm::value_ptr(specular));
}


//
//	OtLightClass::submit
//

void OtLightClass::submit(glm::vec4* slot, const glm::mat4& viewMatrix) {
	slot[0].x = enabled;
	slot[1] = viewMatrix * position;
	slot[2] = diffuse;
	slot[3] = specular;
}


//
//	OtLightClass::getMeta
//

OtType OtLightClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtLightClass>("Light", OtSceneObjectClass::getMeta());
		type->set("setPosition", OtFunctionClass::create(&OtLightClass::setPosition));
		type->set("setDiffuse", OtFunctionClass::create(&OtLightClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtLightClass::setSpecular));
	}

	return type;
}


//
//	OtLightClass::create
//

OtLight OtLightClass::create() {
	OtLight light = std::make_shared<OtLightClass>();
	light->setType(getMeta());
	return light;
}
