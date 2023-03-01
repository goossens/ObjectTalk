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

#include "OtScene.h"
#include "OtColor.h"
#include "OtDirectionalLight.h"
#include "OtViewPort.h"


//
//	Constants
//

constexpr int shadowmapSize = 1024;


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
	return OtObject(this);
}


//
//	OtDirectionalLightClass::setColor
//

OtObject OtDirectionalLightClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return OtObject(this);
}


//
//	OtFixedMaterialClass::setColorRGB
//

OtObject OtDirectionalLightClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return OtObject(this);
}


//
//	OtDirectionalLightClass::addPropertiesToRenderer
//

void OtDirectionalLightClass::addPropertiesToRenderer(OtRenderer &renderer) {
	// handle shadows (if required)
	if (castShadowFlag) {
		csm.update(shadowmapSize, renderer.getCamera(), direction);
		OtRenderer shadowMapRenderer;

		for (auto cascade = 0; cascade < OtCascadedShadowMap::cascades; cascade++) {
			shadowMapRenderer.runShadowPass(
				renderer.getScene(),
				csm.getCascadeCamera(cascade),
				csm.getCascadeFramebuffer(cascade));
		}

		renderer.setCascadedShadowMap(csm);
	}

	// add light properties to renderer
	renderer.setDirectionalLight(direction, color);
}


//
//	OtDirectionalLightClass::renderGUI
//

void OtDirectionalLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);

	ImGui::InputFloat3("Direction", glm::value_ptr(direction));
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
	ImGui::Checkbox("Casts Shadow", &castShadowFlag);

	if (castShadowFlag) {
		if (ImGui::TreeNodeEx("Cascaded Shadows:", ImGuiTreeNodeFlags_Framed)) {
			for (auto cascade = 0; cascade < OtCascadedShadowMap::cascades; cascade++) {
				ImGui::Text("Cascade %d:", cascade);
				float width = ImGui::GetContentRegionAvail().x;
				ImGui::Image((void*)(intptr_t) csm.getCascadeDepthTextureIndex(cascade), ImVec2(width, width));
			}

			ImGui::TreePop();
		}
	}
}


//
//	OtDirectionalLightClass::getMeta
//

OtType OtDirectionalLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtDirectionalLightClass>("DirectionalLight", OtLightClass::getMeta());
		type->set("__init__", OtFunction::create(&OtDirectionalLightClass::init));

		type->set("setDirection", OtFunction::create(&OtDirectionalLightClass::setDirection));
		type->set("setColor", OtFunction::create(&OtDirectionalLightClass::setColor));
		type->set("setColorRGB", OtFunction::create(&OtDirectionalLightClass::setColorRGB));

		type->set("castShadow", OtFunction::create(&OtDirectionalLightClass::castShadow));
		type->set("castsShadow", OtFunction::create(&OtDirectionalLightClass::castsShadow));
	}

	return type;
}
