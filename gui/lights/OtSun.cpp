//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtColor.h"
#include "OtSun.h"


//
//	OtSunClass::init
//

void OtSunClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setAzimuth(parameters[1]->operator float());

		case 1:
			setElevation(parameters[0]->operator float());

		case 0:
			break;

		default:
			OtExcept("[Sun] constructor expects up to 2 arguments (not %ld)", count);
	}
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


//
//	OtSunClass::castShadow
//

OtObject OtSunClass::castShadow(float width, float dist, float near, float far) {
	OtLightClass::castShadow();

	shadowCamera->setOrthographic(width, near, far);
	shadowCamera->setWidthLimits(width / 10.0, width * 10.0);
	shadowCamera->setNearFarLimits(near / 10.0, near * 10.0, far / 10.0, far * 10.0);
	distance = dist;

	return shared();
}


//
//	OtSunClass::getDirectionToSun
//

glm::vec3 OtSunClass::getDirectionToSun() {
	return glm::vec3(
		std::cos(elevation) * std::sin(azimuth),
		std::sin(elevation),
		std::cos(elevation) * -std::cos(azimuth));
}


//
//	OtSunClass::getLightDirection
//

glm::vec3 OtSunClass::getLightDirection() {
	return -getDirectionToSun();
}


//
//	OtSunClass::update
//

void OtSunClass::update(OtRenderingContext context) {
	OtLightClass::update(context);

	// handle shadows (if required)
	if (shadow) {
		// update "light" camera
		auto target = context->getCamera()->getTarget();
		shadowCamera->setPositionVector(target - distance * getDirectionToSun());
		shadowCamera->setTargetVector(target);
		shadowCamera->update();
	}

	// add ambient light
	glm::vec3 ambient = glm::vec3(std::clamp((elevation + 0.1) / 0.3, 0.0, 0.8));
	context->addAmbientLight(ambient);

	// add direct light
	glm::vec3 direct = glm::vec3(0.2 + std::clamp(elevation / 0.1, 0.0, 0.6));
	context->setDirectionalLight(getLightDirection(), direct);
}


//
//	OtSunClass::renderGUI
//

void OtSunClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat("Elevation", &elevation, -0.99, 0.99);
	ImGui::SliderFloat("Azimuth", &azimuth, 0.0, std::numbers::pi * 2.0);

	if (shadowCamera) {
		ImGui::Checkbox("Casts Shadow", &shadow);
	}

	if (shadow) {
		renderShadowCameraGUI();
	}
}


//
//	OtSunClass::getMeta
//

OtType OtSunClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSunClass>("Sun", OtLightClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSunClass::init));
		type->set("setElevation", OtFunctionClass::create(&OtSunClass::setElevation));
		type->set("setAzimuth", OtFunctionClass::create(&OtSunClass::setAzimuth));
		type->set("castShadow", OtFunctionClass::create(&OtSunClass::castShadow));
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
