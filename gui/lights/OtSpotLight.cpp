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
#include "OtNumbers.h"

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
	return OtObject(this);
}


//
//	OtSpotLightClass::setDirection
//

OtObject OtSpotLightClass::setDirection(float x, float y, float z) {
	direction = glm::vec3(x, y, z);
	return OtObject(this);
}


//
//	OtSpotLightClass::setColor
//

OtObject OtSpotLightClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return OtObject(this);
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtSpotLightClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return OtObject(this);
}


//
//	OtSpotLightClass::setAttenuation
//

OtObject OtSpotLightClass::setAttenuation(float constant, float linear, float quadratic) {
	attenuation = glm::vec3(constant, linear, quadratic);
	return OtObject(this);
}


//
//	OtSpotLightClass::setCone
//

OtObject OtSpotLightClass::setCone(float i, float o) {
	inner = i;
	outer = o;
	inner = std::min(inner, outer);
	return OtObject(this);
}


//
//	OtSpotLightClass::addPropertiesToRenderer
//

void OtSpotLightClass::addPropertiesToRenderer(OtRenderer& renderer) {
	renderer.addSpotLight(position, direction, color, attenuation, inner, outer);
}


//
//	OtSpotLightClass::renderGUI
//

void OtSpotLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::InputFloat3("Position", glm::value_ptr(position));
	ImGui::SliderFloat3("Direction", glm::value_ptr(direction), -1.0f, 1.0f);
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
	ImGui::SliderFloat3("Attenuation", glm::value_ptr(attenuation), 0.0, 1.0);
	ImGui::SliderFloat("Outer cone", &outer, 0.0, std::numbers::pi / 2.0);
	ImGui::SliderFloat("Inner cone", &inner, 0.0, outer);
	inner = std::min(inner, outer);
}


//
//	OtSpotLightClass::getMeta
//

OtType OtSpotLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtSpotLightClass>("SpotLight", OtLightClass::getMeta());
		type->set("__init__", OtFunction::create(&OtSpotLightClass::init));

		type->set("setPosition", OtFunction::create(&OtSpotLightClass::setPosition));
		type->set("setDirection", OtFunction::create(&OtSpotLightClass::setDirection));
		type->set("setColor", OtFunction::create(&OtSpotLightClass::setColor));
		type->set("setColorRGB", OtFunction::create(&OtSpotLightClass::setColorRGB));
		type->set("setAttenuation", OtFunction::create(&OtSpotLightClass::setAttenuation));
		type->set("setCone", OtFunction::create(&OtSpotLightClass::setCone));
	}

	return type;
}
