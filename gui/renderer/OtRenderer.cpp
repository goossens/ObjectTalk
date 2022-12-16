//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/ext.hpp"

#include "OtException.h"

#include "OtCamera.h"
#include "OtFramework.h"
#include "OtPass.h"
#include "OtRenderer.h"
#include "OtScene.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"


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
	lightingSlot = shadowMatrixSlot + 4
};

enum {
	lightPositionSlot = 0,
	lightColorSlot,
	lighAttenuationSlot,
	lightConeSlot,
	lastLightPropertiesSlot
};

static const size_t maxUniformSlots = lightingSlot + maxPointSpotLights * lastLightPropertiesSlot;


//
//	OtRenderer::OtRenderer
//

OtRenderer::OtRenderer() {
	// register uniforms
	lightUniform.initialize("u_light", maxUniformSlots);
}


//
//	OtRenderer::runShadowPass
//

void OtRenderer::runShadowPass(OtPass p, OtScene s, OtCamera c, OtFrameBuffer& framebuffer) {
	// remember properties
	renderingPass = shadowmapPass;
	pass = p;
	scene = s;
	camera = c;

	viewX = 0.0;
	viewY = 0.0;
	viewW = framebuffer.getWidth();
	viewH = framebuffer.getHeight();

	// configure rendering pass
	pass.setClear(false, true);
	pass.setRectangle( 0, 0, viewW, viewH);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render shadow
	scene->render(*this);
}


//
//	OtRenderer::runReflectionPass
//

void OtRenderer::runReflectionPass(OtPass p, OtScene s, OtCamera c, OtFrameBuffer& framebuffer) {
	// remember properties
	renderingPass = reflectionPass;
	pass = p;
	scene = s;
	camera = c;

	viewX = 0.0;
	viewY = 0.0;
	viewW = framebuffer.getWidth();
	viewH = framebuffer.getHeight();

	// configure rendering pass
	pass.setClear(true, true);
	pass.setRectangle( 0, 0, viewW, viewH);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render reflection
	scene->render(*this);
}


//
//	OtRenderer::runLightingPass
//

void OtRenderer::runLightingPass(OtScene s, OtCamera c, float x, float y, float w, float h) {
	// remember properties
	renderingPass = lightingPass;
	scene = s;
	camera = c;
	viewX = x;
	viewY = y;
	viewW = w;
	viewH = h;

	// update camera and scene
	camera->update(*this);
	scene->update(*this);

	// configure rendering pass
	pass.reserveRenderingSlot();
	pass.setRectangle(x, y, w, h);
	pass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render scene
	scene->render(*this);

	// render debugging stuff if required
	debugRender();
}


//
//	OtRenderer::setTransform
//

void OtRenderer::setTransform(const glm::mat4& matrix) {
	bgfx::setTransform(glm::value_ptr(matrix));
}


//
//	OtRenderer::setState
//

void OtRenderer::setState(uint64_t state) {
	bgfx::setState(state);
}


//
//	OtRenderer::setDefaultState
//

void OtRenderer::setDefaultState() {
	bgfx::setState(BGFX_STATE_DEFAULT);
}


//
//	OtRenderer::setInstanceData
//

void OtRenderer::setInstanceData(void* data, size_t count, size_t size) {
	bgfx::InstanceDataBuffer idb;
	bgfx::allocInstanceDataBuffer(&idb, (uint32_t) count, (uint16_t) size);
	std::memcpy(idb.data, data, idb.size);
	bgfx::setInstanceDataBuffer(&idb);
}


//
//	OtRenderer::addAmbientLight
//

void OtRenderer::addAmbientLight(const glm::vec3& ambient)
{
	ambientLightColor += ambient;
}

//
//	OtRenderer::setLight
//

void OtRenderer::setDirectionalLight(const glm::vec3& direction, const glm::vec3& color) {
	if (directionalLightEnabled) {
		OtExcept("Directional light can only be set once per scene");
	}

	directionalLightEnabled = true;
	directionalLightDirection = direction;
	directionalLightColor = color;
}


//
//	OtRenderer::addPointLight
//

void OtRenderer::addPointLight(const glm::vec3& position, const glm::vec3& color, const glm::vec3& attenuation) {
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
//	OtRenderer::addSpotLight
//

void OtRenderer::addSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, const glm::vec3& attenuation, float coneInner, float coneOuter) {
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
//	OtRenderer::setFog
//

void OtRenderer::setFog(const glm::vec3& color, float near, float far) {
	fogEnabled = true;
	fogColor = color;
	fogNear = near;
	fogFar = far;
}


//
//	OtRenderer::copyLightProperties
//

void OtRenderer::copyLightProperties(OtRenderer& renderer) {
	ambientLightColor = renderer.ambientLightColor;

	directionalLightEnabled = renderer.directionalLightEnabled;
	directionalLightDirection = renderer.directionalLightDirection;
	directionalLightColor = renderer.directionalLightColor;

	std::memcpy(pointSpotLight, renderer.pointSpotLight, sizeof(pointSpotLight));
	lightCount = renderer.lightCount;

	fogEnabled = renderer.fogEnabled;
	fogColor = renderer.fogColor;
	fogNear = renderer.fogNear;
	fogFar = renderer.fogFar;
}


//
//	OtRenderer::setShadowMap
//

void OtRenderer::setShadowMap(OtFrameBuffer& framebuffer, const glm::mat4& matrix) {
	shadowEnabled = true;
	shadowFrameBuffer = &framebuffer;
	shadowMatrix = matrix;
}


//
//	OtRenderer::submit
//

void OtRenderer::submit(bool receiveShadow) {
	// we only apply shadows if a shadowmap exists and if the object receives the shadow
	bool shadow = shadowEnabled && receiveShadow;

	// setup uniform and submit it to GPU
	glm::vec4* uniform = lightUniform.getValues();
	uniform[cameraPositionSlot] = glm::vec4(camera->getPosition(), 0.0);
	uniform[ambientColorSlot] = glm::vec4(ambientLightColor, 0.0);
	uniform[directionalLightDirectionSlot] = glm::vec4(glm::normalize(-directionalLightDirection), 0.0);
	uniform[directionalLightColorSlot] = glm::vec4(directionalLightColor, 0.0);

	uniform[fogPropertiesSlot] = glm::vec4(fogEnabled, fogNear, fogFar, 0.0);
	uniform[fogColorSlot] = glm::vec4(fogColor, 0.0);

	if (shadow) {
		uniform[shadowPropertiesSlot] = glm::vec4(
			shadow,
			1.0 / shadowFrameBuffer->getWidth(),
			1.0 / shadowFrameBuffer->getHeight(),
			0.0);

		std::memcpy(&uniform[shadowMatrixSlot], glm::value_ptr(shadowMatrix), sizeof(float) * 16);

	} else {
		uniform[shadowPropertiesSlot] = glm::vec4(0.0);
		uniform[shadowMatrixSlot] = glm::vec4(0.0);
		uniform[shadowMatrixSlot + 1] = glm::vec4(0.0);
		uniform[shadowMatrixSlot + 2] = glm::vec4(0.0);
		uniform[shadowMatrixSlot + 3] = glm::vec4(0.0);
	}

	glm::vec4* i = &(uniform[lightingSlot]);

	for (auto c = 0; c < lightCount; c++) {
		*i++ = glm::vec4(pointSpotLight[c].position, 1.0);
		*i++ = glm::vec4(pointSpotLight[c].color, 0.0);
		*i++ = glm::vec4(pointSpotLight[c].attenuation, std::cos(pointSpotLight[c].coneInner));
		*i++ = glm::vec4(pointSpotLight[c].coneDirection, std::cos(pointSpotLight[c].coneOuter));
	}

	lightUniform.submit();

	// bind texture
	if (shadow) {
		shadowFrameBuffer->bindDepthTexture(shadowSampler, 0);

	} else {
		shadowSampler.submit(0);
	}
}


//
//	OtRenderer::debugAxis
//

void OtRenderer::debugAxis(float scale) {
	debugSetColor(0xff0000ff);
	debugMoveTo(glm::vec3(0.0, 0.0, 0.0));
	debugLineTo(glm::vec3(1.0, 0.0, 0.0) * scale);

	debugSetColor(0xff00ff00);
	debugMoveTo(glm::vec3(0.0, 0.0, 0.0));
	debugLineTo(glm::vec3(0.0, 1.0, 0.0) * scale);

	debugSetColor(0xffff0000);
	debugMoveTo(glm::vec3(0.0, 0.0, 0.0));
	debugLineTo(glm::vec3(0.0, 0.0, 0.1) * scale);
}


//
//	OtRenderer::debugLineTo
//

void OtRenderer::debugLineTo(const glm::vec3& point) {
	// add a new line
	auto pos = debugVertices.size();
	debugVertices.emplace_back(debugPoint, debugColor);
	debugVertices.emplace_back(point, debugColor);
	debugIndices.push_back((uint32_t) pos);
	debugIndices.push_back((uint32_t) pos + 1);
	debugPoint = point;
}


//
//	OtRenderer::debugRender
//

void OtRenderer::debugRender() {
	if (debugVertices.size()) {
		OtShader shader = OtShader("OtDebugVS", "OtDebugFS");

		OtTransientVertexBuffer tvb;
		tvb.submit(debugVertices.data(), debugVertices.size(), OtVertexPosCol::getLayout());

		OtTransientIndexBuffer tib;
		tib.submit(debugIndices.data(), debugIndices.size());

		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA |
			BGFX_STATE_PT_LINES |
			BGFX_STATE_LINEAA |
			BGFX_STATE_BLEND_ALPHA);

		runShader(shader);
	}
}
