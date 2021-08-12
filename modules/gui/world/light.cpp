//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <imgui.h>

#include "ot/function.h"

#include "light.h"


//
//	OtLightClass::renderGUI
//

void OtLightClass::renderGUI(int number) {
	int flags = ImGuiTreeNodeFlags_Framed | (active ? ImGuiTreeNodeFlags_DefaultOpen : 0);

	if (ImGui::TreeNodeEx(this, flags, "Light %d", number)) {
		ImGui::Checkbox("active", &active);
		ImGui::SliderFloat3("Position", glm::value_ptr(position), -50.0f, 50.0f);
		ImGui::ColorEdit4("Diffuse", glm::value_ptr(diffuse));
		ImGui::ColorEdit4("Specular", glm::value_ptr(specular));
		ImGui::TreePop();
	}
}


//
//	OtLightClass::submit
//

void OtLightClass::submit(glm::vec4* slot, const glm::mat4& viewMatrix) {
	slot[0].x = active;
	slot[1] = viewMatrix * position;
	slot[2] = diffuse;
	slot[3] = specular;
}


//
//	OtLightClass::getMeta
//

OtType OtLightClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtLightClass>("Light", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtLightClass::init));
		type->set("setPosition", OtFunctionClass::create(&OtLightClass::setPosition));
		type->set("setDiffuse", OtFunctionClass::create(&OtLightClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtLightClass::setSpecular));
	}

	return type;
}


//
//	OtLightClass::create
//

OtLight OtLightClass::create() {
	OtLight light = std::make_shared<OtLightClass>();
	light->setType(getMeta());
	return light;
}
