//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"

#include "renderingcontext.h"


//
//	OtRenderingContext::OtRenderingContext
//

OtRenderingContext::OtRenderingContext(int v, float va, OtScene s, OtCamera c) {
	// set state
	view = v;
	viewAspect = va;
	scene = s;
	camera = c;
	transform = glm::mat4(1.0);

	// set lighting defaults
	for (auto c = 0; c < TOTAL_LIGHT_SLOTS; c++) {
		uniforms[c] = glm::vec4(0.0);
	}

	uniforms[0] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	lights = 0;

	// register uniform
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, TOTAL_LIGHT_SLOTS);
}


//
//	OtRenderingContext::OtRenderingContext
//

OtRenderingContext::~OtRenderingContext() {
	// release resources
	bgfx::destroy(lightUniform);
}


//
//	OtRenderingContext::setAmbientLight
//

void OtRenderingContext::setAmbientLight(const glm::vec3& ambient) {
	uniforms[0] = glm::vec4(ambient, 1.0);
}


//
//	OtRenderingContext::addDirectionalLight
//

void OtRenderingContext::addDirectionalLight(const glm::vec3& dir, const glm::vec3& diffuse, const glm::vec3& specular) {
	// sanity check
	if (lights == DIRECTIONAL_LIGHTS) {
		OtExcept("Too many lights in scene (max %d)", DIRECTIONAL_LIGHTS);
	}

	// store light information
	glm::vec4* p = uniforms + AMBIENT_LIGHT_SLOTS + FOG_SLOTS + lights++ * SLOTS_PER_LIGHT;
	*p++ = glm::vec4(1.0, 0.0, 0.0, 0.0);
	*p++ = glm::vec4(dir, 0.0);
	*p++ = glm::vec4(diffuse, 1.0);
	*p++ = glm::vec4(specular, 1.0);
}


//
//	OtRenderingContext::addPointLight
//

void OtRenderingContext::addPointLight(const glm::vec3& pos, const glm::vec3& diffuse, const glm::vec3& specular) {
	// sanity check

	if (lights == DIRECTIONAL_LIGHTS) {
		OtExcept("Too many lights in scene (max %d)", DIRECTIONAL_LIGHTS);
	}

	// store light information
	glm::vec4* p = uniforms + (AMBIENT_LIGHT_SLOTS + FOG_SLOTS + lights++ * SLOTS_PER_LIGHT);
	*p++ = glm::vec4(1.0, 0.0, 0.0, 0.0);
	*p++ = glm::vec4(pos, 1.0);
	*p++ = glm::vec4(diffuse, 1.0);
	*p++ = glm::vec4(specular, 1.0);
}


//
//	OtRenderingContext::setFog
//

void OtRenderingContext::setFog(const glm::vec3& color, float near, float far) {
	uniforms[1] = glm::vec4(1.0, near, far, 0.0);
	uniforms[2] = glm::vec4(color, 1.0);
}


//
//	OtRenderingContext::submit
//

void OtRenderingContext::submit() {
	bgfx::setUniform(lightUniform, uniforms, TOTAL_LIGHT_SLOTS);
}
