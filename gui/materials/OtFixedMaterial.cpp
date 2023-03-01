//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtColor.h"
#include "OtFixedMaterial.h"
#include "OtFramework.h"


//
//	OtFixedMaterialClass::init
//

void OtFixedMaterialClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setColor(parameters[0]->operator std::string());

	} else if (count == 3) {
		setColorRGB(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 4) {
		setColorRGB(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setOpacity(parameters[3]->operator float());

	} else {
		OtExcept("[FixedMaterial] constructor expects 1, 3 or 4 arguments (not %ld)", count);
	}
}


//
//	OtFixedMaterialClass::setColor
//

OtObject OtFixedMaterialClass::setColor(const std::string& name) {
	// parse CSS style color
	color = OtColorParseToVec3(name);
	return OtObject(this);
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtFixedMaterialClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return OtObject(this);
}


//
//	OtFixedMaterialClass::setOpacity
//

OtObject OtFixedMaterialClass::setOpacity(float o) {
	opacity = o;
	return OtObject(this);
}


void OtFixedMaterialClass::submit(OtRenderer& renderer, bool wireframe, bool instancing) {
	if (renderer.inShadowmapPass()) {
		if (instancing) {
			renderer.runShader(shadowInstancingShader);

		} else {
			renderer.runShader(shadowShader);
		}

	} else {
			// submit uniform
		uniform.set(0, glm::vec4(color, opacity));
		uniform.submit();

		// set rendering state
		renderer.setState(wireframe, frontside, backside, transparent);

		// run appropriate shader
		if (instancing) {
			renderer.runShader(instancingShader);

		} else {
			renderer.runShader(regularShader);
		}
	}
}


//
//	OtFixedMaterialClass::getMeta
//

OtType OtFixedMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtFixedMaterialClass>("FixedMaterial", OtMaterialClass::getMeta());
		type->set("__init__", OtFunction::create(&OtFixedMaterialClass::init));

		type->set("setColor", OtFunction::create(&OtFixedMaterialClass::setColor));
		type->set("setColorRGB", OtFunction::create(&OtFixedMaterialClass::setColorRGB));
		type->set("setOpacity", OtFunction::create(&OtFixedMaterialClass::setOpacity));
	}

	return type;
}
