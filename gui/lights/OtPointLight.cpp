//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtException.h"
#include "OtFunction.h"

#include "OtPointLight.h"
#include "OtColor.h"


//
//	OtPointLightClass::init
//

void OtPointLightClass::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		setPosition(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 4) {
		setPosition(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setColor(parameters[3]->operator std::string());

	} else if (count == 6) {
		setPosition(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setColorRGB(
			parameters[3]->operator float(),
			parameters[4]->operator float(),
			parameters[5]->operator float());

	} else if (count == 7) {
		setPosition(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setColor(parameters[3]->operator std::string());

		setAttenuation(
			parameters[4]->operator float(),
			parameters[5]->operator float(),
			parameters[6]->operator float());

	} else if (count == 9) {
		setPosition(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setColorRGB(
			parameters[3]->operator float(),
			parameters[4]->operator float(),
			parameters[5]->operator float());

		setAttenuation(
			parameters[4]->operator float(),
			parameters[5]->operator float(),
			parameters[6]->operator float());

	} else if (count != 0) {
		OtExcept("[PointLight] constructor expects 0, 3, 4, 6, 7 or 9 arguments (not %ld)", count);
	}
}


//
//	OtPointLightClass::setPosition
//

OtObject OtPointLightClass::setPosition(float x, float y, float z) {
	position = glm::vec3(x, y, z);
	return OtObject(this);
}


//
//	OtPointLightClass::setColor
//

OtObject OtPointLightClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return OtObject(this);
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtPointLightClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return OtObject(this);
}


//
//	OtPointLightClass::setAttenuation
//

OtObject OtPointLightClass::setAttenuation(float constant, float linear, float quadratic) {
	attenuation = glm::vec3(constant, linear, quadratic);
	return OtObject(this);
}


//
//	OtPointLightClass::addPropertiesToRenderer
//

void OtPointLightClass::addPropertiesToRenderer(OtRenderer& renderer) {
	renderer.addPointLight(position, color, attenuation);
}


//
//	OtPointLightClass::renderGUI
//

void OtPointLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::InputFloat3("Position", glm::value_ptr(position));
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
	ImGui::InputFloat3("Attenuation", glm::value_ptr(attenuation));
}


//
//	OtPointLightClass::getMeta
//

OtType OtPointLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPointLightClass>("PointLight", OtLightClass::getMeta());
		type->set("__init__", OtFunction::create(&OtPointLightClass::init));

		type->set("setPosition", OtFunction::create(&OtPointLightClass::setPosition));
		type->set("setColor", OtFunction::create(&OtPointLightClass::setColor));
		type->set("setColorRGB", OtFunction::create(&OtPointLightClass::setColorRGB));
		type->set("setAttenuation", OtFunction::create(&OtPointLightClass::setAttenuation));
	}

	return type;
}
