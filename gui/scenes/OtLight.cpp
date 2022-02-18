//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtFunction.h"

#include "OtColor.h"
#include "OtLight.h"


//
//	OtLightClass::setPosition
//

OtObject OtLightClass::setPosition(float px, float py, float pz) {
	position = glm::vec3(px, py, pz);
	return shared();
}


//
//	OtLightClass::setDiffuse
//

OtObject OtLightClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return shared();
}


//
//	OtLightClass::setSpecular
//

OtObject OtLightClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return shared();
}


//
//	OtLightClass::update
//

void OtLightClass::update(OtRenderingContext context) {
	context->setPointLight(position, diffuse, specular);
}


//
//	OtLightClass::renderGUI
//

void OtLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat3("Position", glm::value_ptr(position), -50.0f, 50.0f);
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(specular));
}


//
//	OtLightClass::getMeta
//

OtType OtLightClass::getMeta() {
	static OtType type;

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
