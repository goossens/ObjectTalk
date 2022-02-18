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
	// register uniforms
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, totalSlots);
	shadowUniform = bgfx::createUniform("u_shadowMatrix", bgfx::UniformType::Mat4);
	shadowmapUniform = bgfx::createUniform("s_shadowmap", bgfx::UniformType::Sampler);
}

OtRenderingContextClass::OtRenderingContextClass(const OtRenderingContextClass &c) {
	// copy properties
	view = c.view;
	viewX = c.viewX;
	viewY = c.viewY;
	viewW = c.viewW;
	viewH = c.viewH;
	viewAspectRatio = c.viewAspectRatio;
	scene = c.scene;
	camera = c.camera;
	transform = c.transform;
	reflection = c.reflection;
	refraction = c.refraction;
	std::copy(std::begin(c.uniforms), std::end(c.uniforms), std::begin(uniforms));
	shadowMatrix  = c.shadowMatrix;

	// register uniforms
	lightUniform = bgfx::createUniform("u_light", bgfx::UniformType::Vec4, totalSlots);
	shadowUniform = bgfx::createUniform("u_shadowMatrix", bgfx::UniformType::Mat4);
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
	transform = glm::mat4(1.0);

	for (auto c = 0; c < totalSlots; c++) {
		uniforms[c] = glm::vec4(0.0);
	}

	uniforms[ambientLightSlot] = glm::vec4(0.0, 0.0, 0.0, 1.0);
}


//
//	OtRenderingContext::setAmbientLight
//

void OtRenderingContextClass::setAmbientLight(const glm::vec3& ambient) {
	uniforms[ambientLightSlot] = glm::vec4(ambient, 1.0);
}


//
//	OtRenderingContext::getAmbientLight
//

glm::vec3 OtRenderingContextClass::getAmbientLight() {
	return glm::vec3(uniforms[ambientLightSlot]);
}


//
//	OtRenderingContext::setDirectionalLight
//

void OtRenderingContextClass::setDirectionalLight(const glm::vec3& dir, const glm::vec3& diffuse, const glm::vec3& specular) {
	// store light information
	uniforms[directionalLightOnOffSlot] = glm::vec4(1.0, 0.0, 0.0, 0.0);
	uniforms[directionalLightDirectionSlot] = glm::vec4(dir, 0.0);
	uniforms[directionalLightDiffuseSlot] = glm::vec4(diffuse, 1.0);
	uniforms[directionalLightSpecularSlot] = glm::vec4(specular, 1.0);
}


//
//	OtRenderingContext::setPointLight
//

void OtRenderingContextClass::setPointLight(const glm::vec3& pos, const glm::vec3& diffuse, const glm::vec3& specular) {
	// store light information
	uniforms[directionalLightOnOffSlot] = glm::vec4(1.0, 0.0, 0.0, 0.0);
	uniforms[directionalLightDirectionSlot] = glm::vec4(pos, 1.0);
	uniforms[directionalLightDiffuseSlot] = glm::vec4(diffuse, 1.0);
	uniforms[directionalLightSpecularSlot] = glm::vec4(specular, 1.0);
}


//
//	OtRenderingContextClass::getCameraFromLightPosition
//

OtCamera OtRenderingContextClass::getCameraFromLightPosition() {
	// create a new camera
	auto lightCamera = OtCameraClass::create();
	auto cameraTarget = camera->getTarget();
	lightCamera->setTargetVector(cameraTarget);

	// do we have a directional light?
	if (uniforms[directionalLightDirectionSlot].w == 0.0) {
		lightCamera->setOrthographic(
			camera->getWidth(),
			camera->getNearClip(),
			camera->getFarClip());

		auto lightPosition = cameraTarget - glm::vec3(uniforms[directionalLightDirectionSlot] * 1000.0f);
		lightCamera->setPositionVector(lightPosition);

	} else {
		lightCamera->setPerspective(
			camera->getFOV(),
			camera->getNearClip(),
			camera->getFarClip());

		lightCamera->setPositionVector(
			glm::vec3(uniforms[directionalLightDirectionSlot])
		);
	}

	return camera;
}


//
//	OtRenderingContextClass::setFog
//

void OtRenderingContextClass::setFog(const glm::vec3& color, float near, float far) {
	uniforms[fogPropertiesSlot] = glm::vec4(1.0, near, far, 0.0);
	uniforms[fogColorSlot] = glm::vec4(color, 1.0);
}


//
//	OtRenderingContextClass::setShadow
//

void OtRenderingContextClass::setShadow(bgfx::TextureHandle texture, const glm::mat4& matrix) {
	uniforms[shadowSlot] = glm::vec4(1.0);
	shadowmapTexture = texture;
	shadowMatrix = matrix;
}


//
//	OtRenderingContextClass::submit
//

void OtRenderingContextClass::submit() {
	uniforms[cameraPositionSlot] = glm::vec4(camera->getPosition(), 0.0);
	bgfx::setUniform(lightUniform, uniforms, totalSlots);
	bgfx::setUniform(shadowUniform, &shadowMatrix);

	if (bgfx::isValid(shadowmapTexture)) {
		bgfx::setTexture(0, shadowmapUniform, shadowmapTexture);

	} else {
		OtTextureClass::dummy()->submit(0, shadowmapUniform);
	}
}
