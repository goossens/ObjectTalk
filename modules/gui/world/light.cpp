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
#include "light.h"


//
//	OtLightClass::setPosition
//

OtObject OtLightClass::setPosition(double px, double py, double pz) {
	position = glm::vec4(px, py, pz, 1.0);
	return shared();
}


//
//	OtLightClass::setDiffuseRGB
//

OtObject OtLightClass::setDiffuseRGB(double r, double g, double b) {
	diffuse = glm::vec4(r, g, b, 1.0);
	return shared();
}


//
//	OtLightClass::setDiffuseCSS
//

OtObject OtLightClass::setDiffuseCSS(const std::string c) {
	diffuse = OtColorParseToVec4(c);
	return shared();
}


//
//	OtLightClass::setSpecularRGB
//

OtObject OtLightClass::setSpecularRGB(double r, double g, double b) {
	specular = glm::vec4(r, g, b, 1.0);
	return shared();
}


//
//	OtLightClass::setSpecularCSS
//

OtObject OtLightClass::setSpecularCSS(const std::string c) {
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
		type->set("setDiffuseRGB", OtFunctionClass::create(&OtLightClass::setDiffuseRGB));
		type->set("setDiffuseCSS", OtFunctionClass::create(&OtLightClass::setDiffuseCSS));
		type->set("setSpecularRGB", OtFunctionClass::create(&OtLightClass::setSpecularRGB));
		type->set("setSpecularCSS", OtFunctionClass::create(&OtLightClass::setSpecularCSS));
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
