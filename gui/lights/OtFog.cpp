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

#include "OtFunction.h"

#include "OtColor.h"
#include "OtFog.h"


//
//	OtFogClass::setColor
//

OtObject OtFogClass::setColor(const std::string& c) {
	color = OtColorParseToVec3(c);
	return shared();
}


//
//	OtFogClass::setDistances
//

OtObject OtFogClass::setDistances(float n, float f) {
	near = std::clamp(n, minNear, maxNear);
	far = std::clamp(f, minFar, maxFar);
	return shared();
}


//
//	OtFogClass::setLimits
//

OtObject OtFogClass::setLimits(float mnn, float mxn, float mnf, float mxf) {
	minNear = mnn;
	maxNear = mxn;
	minFar = mnf;
	maxFar = mxf;
	return shared();
}


//
//	OtFogClass::addPropertiesToRenderer
//

void OtFogClass::addPropertiesToRenderer(OtRenderer& renderer) {
	renderer.setFog(color, near, far);
}


//
//	OtFogClass::renderGUI
//

void OtFogClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
	ImGui::SliderFloat("Near", &near, minNear, maxNear);
	ImGui::SliderFloat("Far", &far, minFar, maxFar);
}


//
//	OtFogClass::getMeta
//

OtType OtFogClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtFogClass>("Fog", OtLightClass::getMeta());
		type->set("setColor", OtFunctionClass::create(&OtFogClass::setColor));
		type->set("setDistances", OtFunctionClass::create(&OtFogClass::setDistances));
		type->set("setLimits", OtFunctionClass::create(&OtFogClass::setLimits));
	}

	return type;
}


//
//	OtFogClass::create
//

OtFog OtFogClass::create() {
	OtFog fog = std::make_shared<OtFogClass>();
	fog->setType(getMeta());
	return fog;
}
