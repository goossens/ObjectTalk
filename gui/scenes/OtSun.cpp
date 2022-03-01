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
//	OtSunClass::OtSunClass
//

OtSunClass::OtSunClass() {
	ambient = OtAmbientClass::create();
	light = OtLightClass::create();
	light->setDiffuseVector(glm::vec3(1.0));
	light->setSpecularVector(glm::vec3(1.0));
}


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
			OtExcept("[Sun] constructor expects up to 2 arguments (not %ld)", count);
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


//
//	OtSunClass::castShadow
//

OtObject OtSunClass::castShadow(float width, float dist, float near, float far, bool debug) {
	distance = dist;
	light->castShadow(width, near, far, debug);
	return shared();
}


//
//	OtSunClass::getDirection
//

glm::vec3 OtSunClass::getDirection() {
	return glm::vec3(
		std::cos(elevation) * std::sin(azimuth),
		std::sin(elevation),
		std::cos(elevation) * -std::cos(azimuth));
}


//
//	OtSunClass::getAmbientLight
//

float OtSunClass::getAmbientLight() {
	return std::clamp((elevation + 0.1) / 0.3, 0.0, 1.0);
}


//
//	OtSunClass::update
//

void OtSunClass::update(OtRenderingContext context) {
	// update ambient light
	ambient->setColorVector(glm::vec3(getAmbientLight()));
	ambient->update(context);

	light->setDirectionVector(getDirection() * distance);
	light->update(context);
}


//
//	OtSunClass::render
//

void OtSunClass::render(OtRenderingContext context) {
	if (!context->inShadowmapPhase()) {
		ambient->render(context);
		light->render(context);
	}
}


//
//	OtSunClass::renderGUI
//

void OtSunClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat("Elevation", &elevation, -0.99, 0.99);
	ImGui::SliderFloat("Azimuth", &azimuth, 0.0, std::numbers::pi * 2.0);
	ImGui::SliderFloat("Distance", &distance, 1.0, 2000.0);

	if (ImGui::TreeNodeEx("Directional Light", ImGuiTreeNodeFlags_Framed)) {
		light->renderGUI();
		ImGui::TreePop();
	}
}


//
//	OtSunClass::getMeta
//

OtType OtSunClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSunClass>("Sun", OtSceneObjectClass::getMeta());
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
