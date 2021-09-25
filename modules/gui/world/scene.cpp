//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "ot/numbers.h"
#include "ot/function.h"

#include "ambient.h"
#include "fog.h"
#include "light.h"
#include "object3d.h"
#include "scene.h"


//
//	Constants
//

#define LIGHTS 4
#define SLOTS_PER_LIGHT 4
#define TOTAL_SLOTS (LIGHTS * SLOTS_PER_LIGHT + 3)


//
//	OtSceneClass::OtSceneClass
//

OtSceneClass::OtSceneClass() {
	// register uniform
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, TOTAL_SLOTS);
}


//
//	OtSceneClass::~OtSceneClass
//

OtSceneClass::~OtSceneClass() {
	// release resources
	bgfx::destroy(lightUniform);
}


//
//	OtSceneClass::validateChild
//

void OtSceneClass::validateChild(OtComponent child) {
	if (!child->isKindOf("SceneObject")) {
		OtExcept("A [Scene] can only have [SceneObject] subclasses as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::render
//

void OtSceneClass::render(int view, OtCamera camera) {
	// light information
	bool hasAmbient = false;
	glm::vec4 uniforms[TOTAL_SLOTS] = { glm::vec4(0.0) };
	glm::vec4* slot = uniforms + 3;
	int lights = 0;

	// list of non-light and non-fog objects
	std::vector<OtSceneObject> objects;

	// collect all light and fog information
	for (auto const& child : children) {
		if (child->isKindOf("Ambient")) {
			hasAmbient = true;
		}

		if (child->isEnabled()) {
			// handle ambient light
			if (child->isKindOf("Ambient")) {
				child->cast<OtAmbientClass>()->submit(uniforms);

			// handle fog
			} else if (child->isKindOf("Fog")) {
				child->cast<OtFogClass>()->submit(uniforms + 1);

			// handle lights
		} else if (child->isKindOf("Light")) {
				if (lights == LIGHTS) {
					OtExcept("Too many lights in scene (max %d)", LIGHTS);
				}

				child->cast<OtLightClass>()->submit(slot, camera);
				slot += SLOTS_PER_LIGHT;
				lights++;

			// handle other scene objects
			} else {
				objects.push_back(child->cast<OtSceneObjectClass>());
			}
		}
	}

	// set default ambient light if required
	if (!hasAmbient) {
		uniforms[0] = glm::vec4(1.0);
	}

	// render all children
	for (auto const& object : objects) {
		bgfx::setUniform(lightUniform, &uniforms, TOTAL_SLOTS);
		object->render(view, camera, glm::mat4(1.0));
	}
}


//
//	OtSceneClass::getMeta
//

OtType OtSceneClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSceneClass>("Scene", OtComponentClass::getMeta());
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
