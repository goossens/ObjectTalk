//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <chrono>
#include <cstdint>

#include "OtPass.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"

#include "OtWaterPass.h"


//
//	OtWaterPass::render
//

void OtWaterPass::render(OtSceneRendererContext& ctx) {
	// update the buffers
	width = ctx.camera.width / 2;
	height = ctx.camera.height / 2;

	renderingBuffer.update(width, height);
	reflectionBuffer.update(width, height);
	refractionBuffer.update(width, height);

	// render the three water passes
	auto& water = ctx.scene->getComponent<OtWaterComponent>(ctx.waterEntity);
	renderReflection(ctx);

	if (water.useRefractance) {
		renderRefraction(ctx);
	}

	renderWater(ctx, water);
}


//
//	OtWaterPass::renderReflection
//

void OtWaterPass::renderReflection(OtSceneRendererContext& ctx) {
	// setup the renderer for the reflection
	auto camera = ctx.camera;
	auto cameraID = ctx.cameraID;
	ctx.camera = ctx.reflectionCamera;
	ctx.cameraID = OtSceneRendererContext::getReflectionCameraID();

	// render the scene
	backgroundReflectionPass.render(ctx);
	if (ctx.hasOpaqueEntities) { deferredReflectionPass.render(ctx); }
	if (ctx.hasTransparentGeometries) { forwardReflectionPass.render(ctx); }
	if (ctx.hasSkyEntities) { skyReflectionPass.render(ctx); }

	ctx.camera = camera;
	ctx.cameraID = cameraID;
}


//
//	OtWaterPass::renderRefraction
//

void OtWaterPass::renderRefraction(OtSceneRendererContext& ctx) {
	// setup the renderer for the refraction
	auto camera = ctx.camera;
	auto cameraID = ctx.cameraID;
	ctx.camera = ctx.refractionCamera;
	ctx.cameraID = OtSceneRendererContext::getRefractionCameraID();

	// render the scene
	backgroundRefractionPass.render(ctx);
	if (ctx.hasOpaqueEntities) { deferredRefractionPass.render(ctx); }
	if (ctx.hasTransparentGeometries) { forwardRefractionPass.render(ctx); }
	if (ctx.hasSkyEntities) { skyRefractionPass.render(ctx); }

	ctx.camera = camera;
	ctx.cameraID = cameraID;
}


//
//	OtWaterPass::renderWater
//

void OtWaterPass::renderWater(OtSceneRendererContext& ctx, OtWaterComponent& water) {
	// setup the rendering pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

	// send out geometry
	static glm::vec3 vertices[] = {
		glm::vec3{-1.0f, -1.0f, 0.0f},
		glm::vec3{1.0f, -1.0f, 0.0f},
		glm::vec3{1.0f, 1.0f, 0.0f},
		glm::vec3{-1.0f, 1.0f, 0.0f}
	};

	OtTransientVertexBuffer vertexBuffer;
	vertexBuffer.submit(vertices, 4, OtVertexPos::getLayout());

	static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
	OtTransientIndexBuffer indexBuffer;
	indexBuffer.submit(indices, 6);

	// determine time
	using namespace std::chrono;
	uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	static uint64_t start = 0;

	if (start == 0) {
		start = now;
	}

	float time = static_cast<float>(static_cast<double>(now - start) / 1000.0) * 0.1f * water.speed;

	// get maximum distance in clip space
	glm::vec4 farPoint = ctx.camera.projectionMatrix * glm::vec4(0.0, water.level, -water.distance, 1.0);
	float distance = farPoint.z / farPoint.w;

	// submit uniforms
	ctx.waterUniforms.setValue(0, water.level, distance, water.depthFactor, 0.0f);
	ctx.waterUniforms.setValue(1, water.scale, static_cast<float>(water.normals.isReady() ? water.normals->getTexture().getWidth() : 1), time, 0.0f);
	ctx.waterUniforms.setValue(2, water.metallic, water.roughness, water.ao, water.reflectivity);
	ctx.waterUniforms.setValue(3, water.color, static_cast<float>(water.useRefractance));
	ctx.waterUniforms.submit();

	ctx.submitLightingUniforms();
	ctx.submitShadowUniforms();

	// bind the textures
	ctx.submitTextureSampler(ctx.waterNormalmapSampler, 0, water.normals);
	reflectionBuffer.bindColorTexture(ctx.reflectionSampler, 1);
	refractionBuffer.bindColorTexture(ctx.refractionSampler, 2);
	refractionBuffer.bindDepthTexture(ctx.refractionDepthSampler, 3);

	// run the waterProgram
	pass.setState(
		OtPass::stateWriteRgb |
		OtPass::stateWriteA |
		OtPass::stateWriteZ |
		OtPass::stateDepthTestLess |
		OtPass::stateBlendAlpha);

	pass.runShaderProgram(waterProgram);
}
