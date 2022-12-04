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

#include "OtException.h"
#include "OtFunction.h"

#include "OtSpotLight.h"
#include "OtColor.h"


//
//	OtSpotLightClass::init
//

void OtSpotLightClass::init(size_t count, OtObject* parameters) {
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
		OtExcept("[SpotLight] constructor expects 0, 3, 4, 6, 7 or 9 arguments (not %ld)", count);
	}
}


//
//	OtSpotLightClass::setPosition
//

OtObject OtSpotLightClass::setPosition(float x, float y, float z) {
	position = glm::vec3(x, y, z);
	return shared();
}


//
//	OtSpotLightClass::setDirection
//

OtObject OtSpotLightClass::setDirection(float x, float y, float z) {
	direction = glm::vec3(x, y, z);
	return shared();
}


//
//	OtSpotLightClass::setColor
//

OtObject OtSpotLightClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtSpotLightClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return shared();
}


//
//	OtSpotLightClass::setAttenuation
//

OtObject OtSpotLightClass::setAttenuation(float constant, float linear, float quadratic) {
	attenuation = glm::vec3(constant, linear, quadratic);
	return shared();
}


//
//	OtSpotLightClass::setCone
//

OtObject OtSpotLightClass::setCone(float i, float o) {
	inner = i;
	outer = o;
	return shared();
}


//
//	OtSpotLightClass::update
//

void OtSpotLightClass::update(OtRenderingContext context) {
	OtLightClass::update(context);
	context->addSpotLight(position, direction, color, attenuation, inner, outer);
}


//
//	OtSpotLightClass::renderGUI
//

void OtSpotLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::InputFloat3("Position", glm::value_ptr(position));
	ImGui::InputFloat3("Direction", glm::value_ptr(direction));
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
	ImGui::InputFloat3("Attenuation", glm::value_ptr(attenuation));
	ImGui::InputFloat("Inner cone", &inner);
	ImGui::InputFloat("Outer cone", &outer);
}


//
//	OtSpotLightClass::getMeta
//

OtType OtSpotLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSpotLightClass>("SpotLight", OtLightClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSpotLightClass::init));

		type->set("setPosition", OtFunctionClass::create(&OtSpotLightClass::setPosition));
		type->set("setDirection", OtFunctionClass::create(&OtSpotLightClass::setDirection));
		type->set("setColor", OtFunctionClass::create(&OtSpotLightClass::setColor));
		type->set("setColorRGB", OtFunctionClass::create(&OtSpotLightClass::setColorRGB));
		type->set("setAttenuation", OtFunctionClass::create(&OtSpotLightClass::setAttenuation));
		type->set("setCone", OtFunctionClass::create(&OtSpotLightClass::setCone));
	}

	return type;
}


//
//	OtSpotLightClass::create
//

OtSpotLight OtSpotLightClass::create() {
	OtSpotLight ambient = std::make_shared<OtSpotLightClass>();
	ambient->setType(getMeta());
	return ambient;
}
