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

#include "OtAABB.h"
#include "OtColor.h"
#include "OtDirectionalLight.h"
#include "OtFrustum.h"


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
//	OtDirectionalLightClass::update
//

void OtDirectionalLightClass::update(OtRenderer& renderer) {
	OtLightClass::update(renderer);

	// handle shadows (if required)
	if (castShadowFlag) {
		// get frustum of regular camera
		OtCamera camera = renderer.getCamera();
		OtFrustum frustum = renderer.getCamera()->getFrustum();

		// calculate center of that frustum
		glm::vec3 center = frustum->getCenter();

		// create light "view" matrix
		glm::mat4 viewMatrix = glm::lookAt(center - direction, center, glm::vec3(0.0, 1.0, 0.0));

		// determine AABB of scene camera frustum in light camera space
		OtAABB aabb = OtAABBClass::create();

		for (auto p = 0; p < frustum->pointCount; p++) {
			aabb->addPoint(viewMatrix * glm::vec4(frustum->getCorner(p), 1.0));
		}

		// get min and max values of AABB
		glm::vec3 min = aabb->getMin();
		glm::vec3 max = aabb->getMax();

		// increase near and far plane of light frustum (to include out of sight objects that create shadows)
		constexpr float factor = 1.2;

		if (min.z < 0) {
			min.z *= factor;

		} else {
			min.z /= factor;
		}

		if (max.z < 0) {
			max.z /= factor;

		} else {
			max.z *= factor;
		}

		// update "light" camera
		shadowCamera->setPositionVector(center - direction);
		shadowCamera->setTargetVector(center);
		shadowCamera->setOrthographicCustom(min.x, max.x, min.y, max.y, min.z, max.z);
		shadowCamera->update(renderer);
	}

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
		if (ImGui::TreeNodeEx("Shadowmap:", ImGuiTreeNodeFlags_Framed)) {
			float width = ImGui::GetContentRegionAvail().x;
			ImGui::Image((void*)(intptr_t) framebuffer.getDepthTextureIndex(), ImVec2(width, width));
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
		type->set("__init__", OtFunctionClass::create(&OtDirectionalLightClass::init));

		type->set("setDirection", OtFunctionClass::create(&OtDirectionalLightClass::setDirection));
		type->set("setColor", OtFunctionClass::create(&OtDirectionalLightClass::setColor));
		type->set("setColorRGB", OtFunctionClass::create(&OtDirectionalLightClass::setColorRGB));

		type->set("castShadow", OtFunctionClass::create(&OtDirectionalLightClass::castShadow));
		type->set("castsShadow", OtFunctionClass::create(&OtDirectionalLightClass::castsShadow));
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
