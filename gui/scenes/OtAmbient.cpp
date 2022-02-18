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

#include "OtAmbient.h"
#include "OtColor.h"


//
//	OtAmbientClass::init
//

OtObject OtAmbientClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setColor(parameters[0]->operator std::string());

	} else if (count != 0) {
		OtExcept("[Ambient] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtAmbientClass::setColor
//

OtObject OtAmbientClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtAmbientClass::update
//

void OtAmbientClass::update(OtRenderingContext context) {
	context->setAmbientLight(color);
}


//
//	OtAmbientClass::renderGUI
//

void OtAmbientClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::ColorEdit3("Ambient", glm::value_ptr(color));
}


//
//	OtAmbientClass::getMeta
//

OtType OtAmbientClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAmbientClass>("Ambient", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtAmbientClass::init));
		type->set("setColor", OtFunctionClass::create(&OtAmbientClass::setColor));
	}

	return type;
}


//
//	OtAmbientClass::create
//

OtAmbient OtAmbientClass::create() {
	OtAmbient ambient = std::make_shared<OtAmbientClass>();
	ambient->setType(getMeta());
	return ambient;
}
