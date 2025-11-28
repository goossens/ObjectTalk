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

#include "OtRenderPass.h"

#include "OtWaterPass.h"

#include "OtWaterVert.h"
#include "OtWaterFrag.h"


//
//	OtWaterPass::render
//

void OtWaterPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// update the buffers
	width = ctx.camera.width / 2;
	height = ctx.camera.height / 2;

	renderingBuffer.update(width, height);
	reflectionBuffer.update(width, height);

	// render the water passes
	auto& water = ctx.scene->getComponent<OtWaterComponent>(ctx.waterEntity);
	renderReflection(ctx);
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
//	OtWaterPass::renderWater
//

void OtWaterPass::renderWater(OtSceneRendererContext& ctx, OtWaterComponent& water) {
	// setup the rendering pass
	OtRenderPass pass;
	pass.start(framebuffer);
	ctx.pass = &pass;

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

	// set vertex uniforms
	struct VertexUniforms {
		glm::mat4 inverseViewProjectionMatrix;
	} vertexUniforms {
		glm::inverse(ctx.camera.viewProjectionMatrix)
	};

	ctx.pass->setVertexUniforms(0, &vertexUniforms, sizeof(vertexUniforms));

	// set fragment uniforms
	struct FragmentUniforms {
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		glm::vec4 waterColor;
		glm::vec2 size;
		float waterLevel;
		float distance;
		float scale;
		float time;
		float metallic;
		float roughness;
		float ao;
		float reflectivity;
	} fragmentUniforms {
		ctx.camera.viewMatrix,
		ctx.camera.viewProjectionMatrix,
		glm::vec4(water.color, 1.0f),
		glm::vec2(static_cast<float>(framebuffer.getWidth()), static_cast<float>(framebuffer.getHeight())),
		water.level,
		distance,
		water.scale,
		time,
		water.metallic,
		water.roughness,
		water.ao,
		water.reflectivity,
	};

	pass.setFragmentUniforms(0, &fragmentUniforms, sizeof(fragmentUniforms));
	ctx.setLightingUniforms(1, 2);
	ctx.setShadowUniforms(2, 5);

	// bind the textures
	ctx.bindFragmentSampler(0, ctx.waterNormalmapSampler, water.normals);
	pass.bindFragmentSampler(1, ctx.reflectionSampler, reflectionBuffer.getColorTexture());

	// render water
	pass.bindPipeline(waterPipeline);
	pass.render(vertexBuffer, indexBuffer);

	// we're done
	pass.end();
}


//
//	OtWaterPass::initializeResources
//

void OtWaterPass::initializeResources() {
	// setup rendering pipeline
	waterPipeline.setShaders(OtWaterVert, sizeof(OtWaterVert), OtWaterFrag, sizeof(OtWaterFrag));
	waterPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	waterPipeline.setVertexDescription(OtVertexPos::getDescription());
	waterPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	// setup vertices
	static glm::vec3 vertices[] = {
		glm::vec3{-1.0f, -1.0f, 0.0f},
		glm::vec3{1.0f, -1.0f, 0.0f},
		glm::vec3{1.0f, 1.0f, 0.0f},
		glm::vec3{-1.0f, 1.0f, 0.0f}
	};

	vertexBuffer.set(vertices, 4, OtVertexPos::getDescription());

	// setup indices
	static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
	indexBuffer.set(indices, 6);
}
