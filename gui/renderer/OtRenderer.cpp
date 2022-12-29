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
#include "OtFormat.h"

#include "OtCamera.h"
#include "OtCylinderGeometry.h"
#include "OtFramework.h"
#include "OtLight.h"
#include "OtRenderer.h"
#include "OtScene.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"


//
//	OtRenderer::OtRenderer
//

OtRenderer::OtRenderer() {
	for (auto i = 0; i < OtCascadedShadowMap::cascades; i++) {
		directionalShadowMapSamplers[i] = OtSampler(OtFormat("s_directional_shadow_map%d", i).c_str());
	}
}


//
//	OtRenderer::reset
//

void OtRenderer::reset() {
	// reset renderer state
	scene = nullptr;
	camera = nullptr;

	ambientLightColor = glm::vec3(0.0);

	directionalLightEnabled = false;
	directionalLightDirection = glm::vec3(0.0);
	directionalLightColor = glm::vec3(0.0);

	for (auto i = 0; i < maxPointSpotLights; i++) {
		pointSpotLight[i] = PointSpotLight();
	}

	lightCount = 0;

	fogEnabled = false;
	fogColor = glm::vec3(1.0);
	fogNear = 0.0;
	fogFar = 0.0;

	csm = nullptr;

	debugColor = 0;
	debugPoint = glm::vec3(0.0);
	debugVertices.clear();
	debugIndices.clear();
}


//
//	OtRenderer::run
//

void OtRenderer::run(OtScene s, OtCamera c, float x, float y, float w, float h) {
	// reset state
	reset();

	// set properties
	renderingPass = lightingPass;
	scene = s;
	camera = c;
	viewX = x;
	viewY = y;
	viewW = w;
	viewH = h;

	// get all light properties
	scene->iterate([this] (OtComponent component) {
		if (component->isKindOf("Light") && component->isEnabled()) {
			component->cast<OtLightClass>()->addPropertiesToRenderer(*this);
		}
	});

	// pre-render all scene components
	scene->iterate([this] (OtComponent component) {
		if (component->isEnabled()) {
			component->cast<OtSceneObjectClass>()->preRender(*this);
		}
	});

	// setup our pass
	pass.reserveRenderingSlot();
	pass.setRectangle(x, y, w, h);
	pass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render scene
	scene->iterate([this] (OtComponent component) {
		if (component->isEnabled()) {
			component->cast<OtSceneObjectClass>()->render(*this);
		}
	});
}

//
//	OtRenderer::runShadowPass
//

void OtRenderer::runShadowPass(OtScene s, OtCamera c, OtFrameBuffer& framebuffer) {
	// set properties
	renderingPass = shadowmapPass;
	scene = s;
	camera = c;

	viewX = 0.0;
	viewY = 0.0;
	viewW = framebuffer.getWidth();
	viewH = framebuffer.getHeight();

	// configure rendering pass
	pass.reserveRenderingSlot();
	pass.setClear(false, true);
	pass.setRectangle(viewX, viewY, viewW, viewH);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render shadow
	scene->iterate([this] (OtComponent component) {
		if (component->isEnabled()) {
			component->cast<OtSceneObjectClass>()->render(*this);
		}
	});

	// reset state
	// reset();
}


//
//	OtRenderer::runReflectionPass
//

void OtRenderer::runReflectionPass(OtScene s, OtCamera c, OtFrameBuffer& framebuffer) {
	// set properties
	renderingPass = reflectionPass;
	scene = s;
	camera = c;

	viewX = 0.0;
	viewY = 0.0;
	viewW = framebuffer.getWidth();
	viewH = framebuffer.getHeight();

	// configure rendering pass
	pass.reserveRenderingSlot();
	pass.setClear(true, true);
	pass.setRectangle( 0, 0, viewW, viewH);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render reflection
	scene->iterate([this] (OtComponent component) {
		if (component->isEnabled()) {
		component->cast<OtSceneObjectClass>()->render(*this);
		}
	});

	// reset state
	reset();
}


//
//	OtRenderer::runDebugPass
//

void OtRenderer::runDebugPass(OtCamera debugCamera, OtScene s, OtCamera viewCamera, OtFrameBuffer &framebuffer) {
	// set properties
	renderingPass = debugPass;
	scene = s;
	camera = debugCamera;

	viewX = 0.0;
	viewY = 0.0;
	viewW = framebuffer.getWidth();
	viewH = framebuffer.getHeight();

	// configure rendering pass
	pass.reserveRenderingSlot();
	pass.setClear(true, true);
	pass.setRectangle( 0, 0, viewW, viewH);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(debugCamera->getViewMatrix(), debugCamera->getProjectionMatrix());

	// render scene
	scene->iterate([this] (OtComponent component) {
		if (component->isEnabled()) {
			component->cast<OtSceneObjectClass>()->render(*this);
		}
	});

	// render debug information
	debugRenderAxis(10.0);
	debugSetColor(0xffff00ff);
	debugRenderCamera(viewCamera, !csm);

	if (csm) {
		debugRenderCascadedShadowMap(csm);
	}

	debugRender();

	// reset state
	reset();
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

void OtRenderer::setState(bool wireframe, bool frontside, bool backside, bool transparent) {
	uint64_t state =
		BGFX_STATE_WRITE_RGB |
		BGFX_STATE_WRITE_A |
		BGFX_STATE_WRITE_Z |
		BGFX_STATE_DEPTH_TEST_LESS |
		BGFX_STATE_MSAA;

	if (wireframe) {
		state |=
			BGFX_STATE_PT_LINES |
			BGFX_STATE_LINEAA |
			BGFX_STATE_BLEND_ALPHA;

	} else {
		if (!frontside) {
			state |= BGFX_STATE_CULL_CCW;
		}

		if (!backside) {
			state |= BGFX_STATE_CULL_CW;
		}

		if (transparent) {
			state |= BGFX_STATE_BLEND_ALPHA;
		}
	}

	bgfx::setState(state);
}


//
//	OtRenderer::setNoDepthState
//

void OtRenderer::setNoDepthState() {
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
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

	pointSpotLight[lightCount].type = spotLight;
	pointSpotLight[lightCount].position = position;
	pointSpotLight[lightCount].color = color;
	pointSpotLight[lightCount].attenuation = attenuation;
	pointSpotLight[lightCount].coneDirection = direction;
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

	csm = renderer.csm;
}


//
//	OtRenderer::setShadowMap
//

void OtRenderer::setCascadedShadowMap(OtCascadedShadowMap& c) {
	csm = &c;
}


//
//	OtRenderer::submit
//

void OtRenderer::submit(bool receiveShadow) {
	// we only apply shadows if a shadowmap exists and if the object receives the shadow
	bool shadow = csm && receiveShadow;

	// setup uniform and submit it to GPU
	glm::vec4* uniform = lightUniform.getValues();
	uniform[cameraPositionSlot] = glm::vec4(camera->getPosition(), 0.0);
	uniform[ambientColorSlot] = glm::vec4(ambientLightColor, 0.0);
	uniform[directionalLightDirectionSlot] = glm::vec4(glm::normalize(-directionalLightDirection), 0.0);
	uniform[directionalLightColorSlot] = glm::vec4(directionalLightColor, 0.0);

	if (shadow) {
		uniform[directionalLightShadowSlot] = glm::vec4(1.0, 1.0 / csm->getSize(), 0.0, 0.0);

		uniform[directionalLightDistancesSlot] = glm::vec4(
			csm->getCascadeFar(0),
			csm->getCascadeFar(1),
			csm->getCascadeFar(2),
			csm->getCascadeFar(3)
		);

	} else {
		uniform[directionalLightShadowSlot] = glm::vec4(0.0);
		uniform[directionalLightDistancesSlot] = glm::vec4(0.0);
	}

	uniform[fogPropertiesSlot] = glm::vec4(fogEnabled, fogNear, fogFar, 0.0);
	uniform[fogColorSlot] = glm::vec4(fogColor, 0.0);

	glm::vec4* i = &(uniform[lightingSlot]);

	for (auto c = 0; c < maxPointSpotLights; c++) {
		*i++ = glm::vec4(pointSpotLight[c].position, c < lightCount ? 1.0 : 0.0);
		*i++ = glm::vec4(pointSpotLight[c].color, 0.0);
		*i++ = glm::vec4(pointSpotLight[c].attenuation, std::cos(pointSpotLight[c].coneInner / 2.0));
		*i++ = glm::vec4(pointSpotLight[c].coneDirection, std::cos(pointSpotLight[c].coneOuter / 2.0));
	}

	lightUniform.submit();

	// bind textures and submit directional shadowmap matrices
	if (shadow) {
		for (auto i = 0; i < OtCascadedShadowMap::cascades; i++) {
			directionalShadowMapSamplers[i].submit(10 + i, csm->getCascadeDepthTexture(i));
			directionalShadowMapTransform.set(i, csm->getCascadeTransform(i));
		}

		directionalShadowMapTransform.submit();

	} else {
		for (auto i = 0; i < OtCascadedShadowMap::cascades; i++) {
			directionalShadowMapSamplers[i].submit(10 + i);
			directionalShadowMapTransform.set(i, glm::mat4(1.0));
		}
	}
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
//	OtRenderer::debugRenderAxis
//

void OtRenderer::debugRenderAxis(float scale) {
	debugSetColor(0xff0000ff);
	debugMoveTo(glm::vec3(0.0, 0.0, 0.0));
	debugLineTo(glm::vec3(1.0, 0.0, 0.0) * scale);

	debugSetColor(0xff00ff00);
	debugMoveTo(glm::vec3(0.0, 0.0, 0.0));
	debugLineTo(glm::vec3(0.0, 1.0, 0.0) * scale);

	debugSetColor(0xffff8000);
	debugMoveTo(glm::vec3(0.0, 0.0, 0.0));
	debugLineTo(glm::vec3(0.0, 0.0, 1.0) * scale);
}


//
//	OtRenderer::debugRenderAABB
//

void OtRenderer::debugRenderAABB(const OtAABB& aabb) {
	auto minp = aabb.getMin();
	auto maxp = aabb.getMax();

	glm::vec3 corners[8] = {
		{ minp.x, minp.y, minp.z },
		{ minp.x, minp.y, maxp.z },
		{ minp.x, maxp.y, maxp.z },
		{ minp.x, maxp.y, minp.z },
		{ maxp.x, minp.y, minp.z },
		{ maxp.x, minp.y, maxp.z },
		{ maxp.x, maxp.y, maxp.z },
		{ maxp.x, maxp.y, minp.z }
	};

	debugMoveTo(corners[0]);
	debugLineTo(corners[1]);
	debugLineTo(corners[2]);
	debugLineTo(corners[3]);
	debugLineTo(corners[0]);

	debugMoveTo(corners[4]);
	debugLineTo(corners[5]);
	debugLineTo(corners[6]);
	debugLineTo(corners[7]);
	debugLineTo(corners[4]);

	debugMoveTo(corners[0]);
	debugLineTo(corners[4]);

	debugMoveTo(corners[1]);
	debugLineTo(corners[5]);

	debugMoveTo(corners[2]);
	debugLineTo(corners[6]);

	debugMoveTo(corners[3]);
	debugLineTo(corners[7]);
}


//
//	OtRenderer::debugRenderFrustum
//

void OtRenderer::debugRenderFrustum(const OtFrustum& frustum) {
	debugMoveTo(frustum.getCorner(OtFrustum::nearBottomLeft));
	debugLineTo(frustum.getCorner(OtFrustum::nearBottomRight));
	debugLineTo(frustum.getCorner(OtFrustum::nearTopRight));
	debugLineTo(frustum.getCorner(OtFrustum::nearTopLeft));
	debugLineTo(frustum.getCorner(OtFrustum::nearBottomLeft));

	debugMoveTo(frustum.getCorner(OtFrustum::farBottomLeft));
	debugLineTo(frustum.getCorner(OtFrustum::farBottomRight));
	debugLineTo(frustum.getCorner(OtFrustum::farTopRight));
	debugLineTo(frustum.getCorner(OtFrustum::farTopLeft));
	debugLineTo(frustum.getCorner(OtFrustum::farBottomLeft));

	debugMoveTo(frustum.getCorner(OtFrustum::nearBottomLeft));
	debugLineTo(frustum.getCorner(OtFrustum::farBottomLeft));

	debugMoveTo(frustum.getCorner(OtFrustum::nearBottomRight));
	debugLineTo(frustum.getCorner(OtFrustum::farBottomRight));

	debugMoveTo(frustum.getCorner(OtFrustum::nearTopRight));
	debugLineTo(frustum.getCorner(OtFrustum::farTopRight));

	debugMoveTo(frustum.getCorner(OtFrustum::nearTopLeft));
	debugLineTo(frustum.getCorner(OtFrustum::farTopLeft));
}


//
//	OtRenderer::debugRenderCamera
//

void OtRenderer::debugRenderCamera(OtCamera camera, bool frustum) {
	if (frustum) {
		debugRenderFrustum(camera->getFrustum());
	}

	auto cone = OtCylinderGeometryClass::create();
	cone->setTopRadius(0.0);
	cone->setBottomRadius(1.0);
	cone->setHeight(1.0);

	debugRenderGeometry(
		cone,
		glm::rotate(
			glm::inverse(camera->getViewMatrix()),
			(float) (std::numbers::pi / 2.0),
			glm::vec3(1.0, 0.0, 0.0)));
}


//
//	OtRenderer::debugRenderCascadedShadowMap
//

void OtRenderer::debugRenderCascadedShadowMap(OtCascadedShadowMap* cascadedShadowMap) {
	debugSetColor(0xffff00ff);

	for (auto i = 0; i < OtCascadedShadowMap::cascades; i++) {
		debugRenderFrustum(cascadedShadowMap->getCascadeFrustum(i));
	}

	debugSetColor(0xff00ffff);

	for (auto i = 0; i < OtCascadedShadowMap::cascades; i++) {
		debugRenderFrustum(cascadedShadowMap->getCascadeCamera(i)->getFrustum());
	}
}


//
//	OtRenderer::debugRenderGeometry
//

void OtRenderer::debugRenderGeometry(OtGeometry geometry, const glm::mat4& matrix) {
	auto vertices = geometry->getVertexes();
	auto lines = geometry->getLines();
	auto count = geometry->getLineIndexCount();

	for (auto i = 0; i < count; i += 2) {
		debugMoveTo(matrix * glm::vec4(vertices[lines[i]].position, 1.0));
		debugLineTo(matrix * glm::vec4(vertices[lines[i + 1]].position, 1.0));
	}
}


//
//	OtRenderer::debugRender
//

void OtRenderer::debugRender()
{
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
