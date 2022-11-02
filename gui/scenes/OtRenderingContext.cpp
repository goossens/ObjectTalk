//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "glm/ext.hpp"

#include "OtException.h"

#include "OtCamera.h"
#include "OtRenderingContext.h"
#include "OtTexture.h"


//
//	OtRenderingContextClass::OtRenderingContextClass
//

OtRenderingContextClass::OtRenderingContextClass() {
	// register lightUniforms
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, totalLightSlots);
	shadowUniform = bgfx::createUniform("u_shadow", bgfx::UniformType::Vec4, totalShadowSlots);
	shadowmapUniform = bgfx::createUniform("s_shadowmap", bgfx::UniformType::Sampler);
}

OtRenderingContextClass::OtRenderingContextClass(const OtRenderingContextClass &c) {
	// copy properties
	phase = c.phase;
	view = c.view;
	viewX = c.viewX;
	viewY = c.viewY;
	viewW = c.viewW;
	viewH = c.viewH;
	viewAspectRatio = c.viewAspectRatio;
	scene = c.scene;
	camera = c.camera;
	std::copy(std::begin(c.lightUniforms), std::end(c.lightUniforms), std::begin(lightUniforms));
	shadowMatrix  = c.shadowMatrix;

	// register lightUniforms
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, totalLightSlots);
	shadowUniform = bgfx::createUniform("u_shadow", bgfx::UniformType::Vec4, totalShadowSlots);
	shadowmapUniform = bgfx::createUniform("s_shadowmap", bgfx::UniformType::Sampler);
}


//
//	OtRenderingContextClass::~OtRenderingContextClass
//

OtRenderingContextClass::~OtRenderingContextClass() {
	// release resources
	bgfx::destroy(lightUniform);
	bgfx::destroy(shadowUniform);
	bgfx::destroy(shadowmapUniform);
}


//
//	OtRenderingContextClass::clear
//

void OtRenderingContextClass::clear() {
	for (auto c = 0; c < totalLightSlots; c++) {
		lightUniforms[c] = glm::vec4(0.0);
	}

	lightUniforms[ambientSlot] = glm::vec4(0.0, 0.0, 0.0, 1.0);
}


//
//	OtRenderingContext::setAmbientLight
//

void OtRenderingContextClass::setAmbientLight(const glm::vec3& ambient) {
	lightUniforms[ambientSlot] = glm::vec4(ambient, 1.0);
}


//
//	OtRenderingContext::getAmbientLight
//

glm::vec3 OtRenderingContextClass::getAmbientLight() {
	return glm::vec3(lightUniforms[ambientSlot]);
}


//
//	OtRenderingContext::setLight
//

void OtRenderingContextClass::setLight(const glm::vec3& direction, const glm::vec3& diffuse, const glm::vec3& specular) {
	// store light information
	lightUniforms[lightOnOffSlot] = glm::vec4(1.0, 0.0, 0.0, 0.0);
	lightUniforms[lightPositionSlot] = glm::normalize(glm::vec4(direction, 0.0));
	lightUniforms[lightDiffuseSlot] = glm::vec4(diffuse, 1.0);
	lightUniforms[lightSpecularSlot] = glm::vec4(specular, 1.0);
}


//
//	OtRenderingContextClass::setFog
//

void OtRenderingContextClass::setFog(const glm::vec3& color, float near, float far) {
	lightUniforms[fogPropertiesSlot] = glm::vec4(1.0, near, far, 0.0);
	lightUniforms[fogColorSlot] = glm::vec4(color, 1.0);
}


//
//	OtRenderingContextClass::setShadowMap
//

void OtRenderingContextClass::setShadowMap(bgfx::TextureHandle texture, const glm::mat4& matrix) {
	shadowmapTexture = texture;
	shadowMatrix = matrix;
}


//
//	OtRenderingContextClass::submit
//

void OtRenderingContextClass::submit(bool receiveShadow) {
	lightUniforms[cameraPositionSlot] = glm::vec4(camera->getPosition(), 0.0);
	bgfx::setUniform(lightUniform, lightUniforms, totalLightSlots);

	int i = sizeof(glm::mat4);
	memcpy(&shadowUniforms[shadowMatrixSlot], glm::value_ptr(shadowMatrix), sizeof(float) * 16);

	if (receiveShadow && bgfx::isValid(shadowmapTexture)) {
		shadowUniforms[shadowEnabledSlot] = glm::vec4(1.0, 1.0 / 512.0, 1.0 / 512.0, 0.0);
		bgfx::setTexture(0, shadowmapUniform, shadowmapTexture);

	} else {
		shadowUniforms[shadowEnabledSlot] = glm::vec4(0.0);
		OtTextureClass::dummy()->submit(0, shadowmapUniform);
	}

	bgfx::setUniform(shadowUniform, shadowUniforms, totalShadowSlots);
}
