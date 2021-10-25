//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/numbers.h"
#include "ot/function.h"

#include "ambient.h"
#include "sun.h"
#include "fog.h"
#include "light.h"
#include "object3d.h"
#include "scene.h"


//
//	Constants
//

#define AMBIENT_SLOTS 1
#define FOG_SLOTS 2

#define LIGHTS 4
#define SLOTS_PER_LIGHT 4

#define FIXED_SLOTS (AMBIENT_SLOTS + FOG_SLOTS)
#define TOTAL_SLOTS (FIXED_SLOTS + LIGHTS * SLOTS_PER_LIGHT)


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
		OtExcept("A [Scene] can only have [SceneObjects] as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::preRender
//

void OtSceneClass::preRender(OtCamera camera, float viewAspect) {

	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->preRender(cast<OtSceneClass>(), viewAspect, camera);
		}
	}
}


//
//	OtSceneClass::render
//

void OtSceneClass::render(int view, OtCamera camera, float viewAspect) {
	// setup camera
	camera->submit(view, viewAspect);

	// light information
	glm::vec4 uniforms[TOTAL_SLOTS] = { glm::vec4(0.0) };
	glm::vec4* lightSlot = uniforms + FIXED_SLOTS;
	int lights = 0;

	// render all scene objects
	for (auto const& child : children) {
		if (child->isEnabled()) {
			// handle ambient light
			if (child->isKindOf("Ambient")) {
				child->cast<OtAmbientClass>()->submit(uniforms);

			// handle fog
			} else if (child->isKindOf("Fog")) {
				child->cast<OtFogClass>()->submit(uniforms + AMBIENT_SLOTS);

			// handle sun
			} else if (child->isKindOf("Sun")) {
				if (lights == LIGHTS) {
					OtExcept("Too many lights in scene (max %d)", LIGHTS);
				}

				child->cast<OtSunClass>()->submit(lightSlot, camera);
				lightSlot += SLOTS_PER_LIGHT;
				lights++;

			// handle lights
			} else if (child->isKindOf("Light")) {
				if (lights == LIGHTS) {
					OtExcept("Too many lights in scene (max %d)", LIGHTS);
				}

				child->cast<OtLightClass>()->submit(lightSlot, camera);
				lightSlot += SLOTS_PER_LIGHT;
				lights++;

			// handle other scene objects
			} else {
				bgfx::setUniform(lightUniform, &uniforms, TOTAL_SLOTS);
				child->cast<OtSceneObjectClass>()->render(view, camera, glm::mat4(1.0));
			}
		}
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
