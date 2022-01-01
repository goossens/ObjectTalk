//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "color.h"
#include "sun.h"


//
//	OtSunClass::init
//

OtObject OtSunClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setAzimuth(parameters[1]->operator float());

		case 1:
			setElevation(parameters[0]->operator float());

		case 0:
			break;

		default:
			OtExcept("[Sky] constructor expects up to 2 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtSunClass::setElevation
//

OtObject OtSunClass::setElevation(float e) {
	elevation = e;
	return shared();
}


//
//	OtSunClass::setAzimuth
//

OtObject OtSunClass::setAzimuth(float a) {
	azimuth = a;
	return shared();
}


glm::vec3 OtSunClass::getDirection() {
	return glm::vec3(
		std::cos(elevation) * std::sin(azimuth),
		std::sin(elevation),
		std::cos(elevation) * -std::cos(azimuth));
}


//
//	OtSunClass::renderGUI
//

void OtSunClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat("Elevation", &elevation, -0.99, 0.99);
	ImGui::SliderFloat("Azimuth", &azimuth, 0.0, std::numbers::pi * 2.0);
}


//
//	OtLightClass::render
//

void OtSunClass::render(OtRenderingContext* context) {
	float ambient = std::clamp((elevation + 0.1) / 0.3, 0.0, 1.0);
	context->setAmbientLight(glm::vec3(ambient));

	glm::vec3 direction = glm::normalize(context->camera->getViewMatrix() * glm::vec4(getDirection(), 0.0));
	context->addDirectionalLight(direction, glm::vec3(1.0), glm::vec3(1.0));
}


//
//	OtSunClass::getMeta
//

OtType OtSunClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSunClass>("Sun", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSunClass::init));
		type->set("setElevation", OtFunctionClass::create(&OtSunClass::setElevation));
		type->set("setAzimuth", OtFunctionClass::create(&OtSunClass::setAzimuth));
	}

	return type;
}


//
//	OtSunClass::create
//

OtSun OtSunClass::create() {
	OtSun sun = std::make_shared<OtSunClass>();
	sun->setType(getMeta());
	return sun;
}
