//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>
#include <cmath>

#include "glm/ext.hpp"

#include "OtException.h"

#include "OtCamera.h"
#include "OtFramework.h"
#include "OtRenderingContext.h"


//
//	Light uniform layout
//

enum {
	cameraPositionSlot = 0,
	ambientColorSlot,
	directionalLightDirectionSlot,
	directionalLightColorSlot,
	fogPropertiesSlot,
	fogColorSlot,
	shadowPropertiesSlot,
	shadowMatrixSlot,
	lightingSlotCount = shadowMatrixSlot + 4
};

enum {
	lightPositionSlot = 0,
	lightColorSlot,
	lighAttenuationSlot,
	lightConeSlot,
	lastLightPropertiesSlot
};

static const size_t maxUniformSlots = lightingSlotCount + maxPointSpotLights * lastLightPropertiesSlot;


//
//	OtRenderingContextClass::OtRenderingContextClass
//

OtRenderingContextClass::OtRenderingContextClass() {
	// register uniforms
	auto framework = OtFrameworkClass::instance();
	lightUniform = framework->getUniform("u_light", bgfx::UniformType::Vec4, maxUniformSlots);
	shadowmapUniform = framework->getUniform("s_shadowmap", bgfx::UniformType::Sampler);
}


//
//	OtRenderingContext::addAmbientLight
//

void OtRenderingContextClass::addAmbientLight(const glm::vec3& ambient) {
	ambientLightColor += ambient;
}


//
//	OtRenderingContext::setLight
//

void OtRenderingContextClass::setDirectionalLight(const glm::vec3& direction, const glm::vec3& color) {
	if (directionalLightEnabled) {
		OtExcept("Directional light can only be set once per scene");
	}

	directionalLightEnabled = true;
	directionalLightDirection = direction;
	directionalLightColor = color;
}


//
//	OtRenderingContextClass::addPointLight
//

void OtRenderingContextClass::addPointLight(const glm::vec3& position, const glm::vec3& color, const glm::vec3& attenuation) {
	if (lightCount == maxPointSpotLights) {
		OtExcept("Too many point/spot lights in scene (max %d)", maxPointSpotLights);
	}

	pointSpotLight[lightCount].type = pointLight;
	pointSpotLight[lightCount].position = position;
	pointSpotLight[lightCount].color = color;
	pointSpotLight[lightCount].attenuation = attenuation;
	lightCount++;
}


//
//	OtRenderingContextClass::addSpotLight
//

void OtRenderingContextClass::addSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, const glm::vec3& attenuation, float coneInner, float coneOuter) {
	if (lightCount == maxPointSpotLights) {
		OtExcept("Too many point/spot lights in scene (max %d)", maxPointSpotLights);
	}

	pointSpotLight[lightCount].type = pointLight;
	pointSpotLight[lightCount].position = position;
	pointSpotLight[lightCount].color = color;
	pointSpotLight[lightCount].attenuation = attenuation;
	pointSpotLight[lightCount].coneInner = coneInner;
	pointSpotLight[lightCount].coneOuter = coneOuter;

	lightCount++;
}


//
//	OtRenderingContextClass::setFog
//

void OtRenderingContextClass::setFog(const glm::vec3& color, float near, float far) {
	fogEnabled = true;
	fogColor = color;
	fogNear = near;
	fogFar = far;
}


//
//	OtRenderingContextClass::copyLightProperties
//

void OtRenderingContextClass::copyLightProperties(OtRenderingContext context) {
	ambientLightColor = context->ambientLightColor;

	directionalLightEnabled = context->directionalLightEnabled;
	directionalLightDirection = context->directionalLightDirection;
	directionalLightColor = context->directionalLightColor;

	std::memcpy(pointSpotLight, context->pointSpotLight, sizeof(pointSpotLight));
	lightCount = context->lightCount;

	fogEnabled = context->fogEnabled;
	fogColor = context->fogColor;
	fogNear = context->fogNear;
	fogFar = context->fogFar;
}


//
//	OtRenderingContextClass::setShadowMap
//

void OtRenderingContextClass::setShadowMap(bgfx::TextureHandle texture, const glm::mat4& matrix) {
	shadowEnabled = true;
	shadowmapTexture = texture;
	shadowMatrix = matrix;
}


//
//	OtRenderingContextClass::submit
//

void OtRenderingContextClass::submit(bool receiveShadow) {
	// we only apply shadows if a shadowmap exists and if the object receives the shadow
	bool shadow = shadowEnabled && receiveShadow;

	// setup uniforms
	glm::vec4 uniforms[maxUniformSlots];
	std::memset(uniforms, 0, sizeof(uniforms));

	uniforms[cameraPositionSlot] = glm::vec4(camera->getPosition(), 0.0);
	uniforms[ambientColorSlot] = glm::vec4(ambientLightColor, 0.0);
	uniforms[directionalLightDirectionSlot] = glm::vec4(glm::normalize(-directionalLightDirection), 0.0);
	uniforms[directionalLightColorSlot] = glm::vec4(directionalLightColor, 0.0);

	uniforms[fogPropertiesSlot] = glm::vec4(fogEnabled, fogNear, fogFar, 0.0);
	uniforms[fogColorSlot] = glm::vec4(fogColor, 0.0);

	uniforms[shadowPropertiesSlot] = glm::vec4(shadow, 1.0 / 512.0, 1.0 / 512.0, 0.0);
	std::memcpy(&uniforms[shadowMatrixSlot], glm::value_ptr(shadowMatrix), sizeof(float) * 16);

	glm::vec4* i = &(uniforms[lightingSlotCount]);

	for (auto c = 0; c < lightCount; c++) {
		*i++ = glm::vec4(pointSpotLight[c].position, 1.0);
		*i++ = glm::vec4(pointSpotLight[c].color, 0.0);
		*i++ = glm::vec4(pointSpotLight[c].attenuation, std::cos(pointSpotLight[c].coneInner));
		*i++ = glm::vec4(pointSpotLight[c].coneDirection, std::cos(pointSpotLight[c].coneOuter));
	}

	bgfx::setUniform(lightUniform, uniforms, maxUniformSlots);

	if (shadow) {
		bgfx::setTexture(0, shadowmapUniform, shadowmapTexture);

	} else {
		bgfx::setTexture(0, shadowmapUniform, OtFrameworkClass::instance()->getDummyTexture());
	}
}
