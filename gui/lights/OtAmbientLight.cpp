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

#include "OtAmbientLight.h"
#include "OtColor.h"


//
//	OtAmbientLightClass::init
//

void OtAmbientLightClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setColor(parameters[0]->operator std::string());

	} else if (count == 3) {
		setColorRGB(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count != 0) {
		OtExcept("[AmbientLight] constructor expects 0, 1 or 3 arguments (not %ld)", count);
	}
}


//
//	OtAmbientLightClass::setColor
//

OtObject OtAmbientLightClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtAmbientLightClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return shared();
}


//
//	OtAmbientLightClass::addPropertiesToRenderer
//

void OtAmbientLightClass::addPropertiesToRenderer(OtRenderer& renderer) {
	renderer.addAmbientLight(color);
}


//
//	OtAmbientLightClass::renderGUI
//

void OtAmbientLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::ColorEdit3("Ambient", glm::value_ptr(color));
}


//
//	OtAmbientLightClass::getMeta
//

OtType OtAmbientLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAmbientLightClass>("AmbientLight", OtLightClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtAmbientLightClass::init));

		type->set("setColor", OtFunctionClass::create(&OtAmbientLightClass::setColor));
		type->set("setColorRGB", OtFunctionClass::create(&OtAmbientLightClass::setColorRGB));
	}

	return type;
}


//
//	OtAmbientLightClass::create
//

OtAmbientLight OtAmbientLightClass::create() {
	OtAmbientLight ambient = std::make_shared<OtAmbientLightClass>();
	ambient->setType(getMeta());
	return ambient;
}
