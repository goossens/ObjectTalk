//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"
#include "imgui.h"

#include "OtException.h"
#include "OtFunction.h"

#include "OtDirectionalLight.h"
#include "OtColor.h"


//
//	OtDirectionalLightClass::init
//

void OtDirectionalLightClass::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		setDirection(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 4) {
		setDirection(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setColor(parameters[3]->operator std::string());

	} else if (count == 6) {
		setDirection(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setColorRGB(
			parameters[3]->operator float(),
			parameters[4]->operator float(),
			parameters[5]->operator float());

	} else if (count != 0) {
		OtExcept("[AmbientLight] constructor expects 0, 3, 4 or 6 arguments (not %ld)", count);
	}
}


//
//	OtDirectionalLightClass::setDirection
//

OtObject OtDirectionalLightClass::setDirection(float x, float y, float z) {
	direction = glm::vec3(x, y, z);
	return shared();
}


//
//	OtDirectionalLightClass::setColor
//

OtObject OtDirectionalLightClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtDirectionalLightClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return shared();
}


//
//	OtDirectionalLightClass::castShadow
//

OtObject OtDirectionalLightClass::castShadow(float width, float near, float far) {
	OtLightClass::castShadow();

	shadowCamera->setOrthographic(width, near, far);
	shadowCamera->setWidthLimits(width / 10.0, width * 10.0);
	shadowCamera->setNearFarLimits(near / 10.0, near * 10.0, far / 10.0, far * 10.0);

	return shared();
}


//
//	OtDirectionalLightClass::update
//

void OtDirectionalLightClass::update(OtRenderingContext context) {
	OtLightClass::update(context);

	// handle shadow (if required)
	if (shadow) {
		// update "light" camera
		auto target = context->getCamera()->getTarget();
		shadowCamera->setPositionVector(target - direction);
		shadowCamera->setTargetVector(target);
		shadowCamera->update();
	}

	context->setDirectionalLight(direction, color);
}


//
//	OtDirectionalLightClass::renderGUI
//

void OtDirectionalLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);

	ImGui::InputFloat3("Direction", glm::value_ptr(direction));
	ImGui::ColorEdit3("Color", glm::value_ptr(color));

	if (shadowCamera) {
		ImGui::Checkbox("Casts Shadow", &shadow);
	}

	if (shadow) {
		renderShadowCameraGUI();
	}
}


//
//	OtDirectionalLightClass::getMeta
//

OtType OtDirectionalLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtDirectionalLightClass>("DirectionalLight", OtLightClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtDirectionalLightClass::init));

		type->set("setDirection", OtFunctionClass::create(&OtDirectionalLightClass::setDirection));
		type->set("setColor", OtFunctionClass::create(&OtDirectionalLightClass::setColor));
		type->set("setColorRGB", OtFunctionClass::create(&OtDirectionalLightClass::setColorRGB));

		type->set("castShadow", OtFunctionClass::create(&OtDirectionalLightClass::castShadow));
	}

	return type;
}


//
//	OtDirectionalLightClass::create
//

OtDirectionalLight OtDirectionalLightClass::create() {
	OtDirectionalLight ambient = std::make_shared<OtDirectionalLightClass>();
	ambient->setType(getMeta());
	return ambient;
}
