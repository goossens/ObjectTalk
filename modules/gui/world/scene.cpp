//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <imgui.h>

#include "ot/numbers.h"
#include "ot/function.h"

#include "object3d.h"
#include "scene.h"


//
//	Constants
//

#define LIGHTS 3
#define SLOTS_PER_LIGHT 4
#define TOTAL_SLOTS (LIGHTS * SLOTS_PER_LIGHT + 1)


//
//	OtSceneClass::OtSceneClass
//

OtSceneClass::OtSceneClass() {
	// register uniform
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, TOTAL_SLOTS);

	// create default fog
	fog = OtFogClass::create();
}


//
//	OtSceneClass::validateChild
//

void OtSceneClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Object3D")) {
		OtExcept("A [Scene] can only have [Object3D] subclasses as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::~OtSceneClass
//

OtSceneClass::~OtSceneClass() {
	// release resources
	if (lightUniform.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(lightUniform);
	}
}


//
//	OtSceneClass::setFog
//

void OtSceneClass::setFog(OtObject fg) {
	// ensure object is fog
	if (fg->isKindOf("Fog")) {
		fog = fg->cast<OtFogClass>();

	} else {
		OtExcept("Expected a [Fog] object, not a [%s]", fg->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::addLight
//

void OtSceneClass::addLight(OtObject light) {
	// sanity check
	if (lights.size() >= LIGHTS) {
		OtExcept("Too many point/spot lights in scene (max %d)", LIGHTS);
	}

	// ensure object is a light
	if (light->isKindOf("Light")) {
		lights.push_back(light->cast<OtLightClass>());

	} else {
		OtExcept("Expected a [Light] object, not a [%s]", light->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::renderLightGUI
//

void OtSceneClass::renderLightGUI() {
	int flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx(this, flags, "Lights:")) {
		ImGui::ColorEdit4("Ambient", glm::value_ptr(ambient));

		int i = 1;

		for (auto& light : lights) {
			light->renderGUI(i++);
		}

		ImGui::TreePop();
	}
}


//
//	OtSceneClass::render
//

void OtSceneClass::render(int view, const glm::mat4& viewMatrix) {
	// pass fog information
	fog->submit();

	// pass light information
	glm::vec4 uniforms[TOTAL_SLOTS] = { glm::vec4(0.0) };
	glm::vec4* slot = uniforms +1;

	for (auto& light : lights) {
		light->submit(slot, viewMatrix);
		slot += SLOTS_PER_LIGHT;
	}

	uniforms[0] = ambient;
	bgfx::setUniform(lightUniform, &uniforms, TOTAL_SLOTS);

	// render all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtObject3dClass>()->render(view, glm::mat4(1.0));
		}
	}
}


//
//	OtSceneClass::getMeta
//

OtType OtSceneClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSceneClass>("Scene", OtObject3dClass::getMeta());
		type->set("setFog", OtFunctionClass::create(&OtSceneClass::setFog));
		type->set("setAmbientLight", OtFunctionClass::create(&OtSceneClass::setAmbientLight));
		type->set("addLight", OtFunctionClass::create(&OtSceneClass::addLight));
	}

	return type;
}


//
//	OtSceneClass::create
//

OtScene OtSceneClass::create() {
	OtScene scene = std::make_shared<OtSceneClass>();
	scene->setType(getMeta());
	return scene;
}
