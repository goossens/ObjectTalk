//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "imgui.h"

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
	refractionBuffer.update(width, height);

	// render the water passes
	auto& water = ctx.scene->getComponent<OtWaterComponent>(ctx.waterEntity);
	renderReflection(ctx, water);

	if (water.useRefractance) {
		renderRefraction(ctx, water);
	}

	renderWater(ctx, water);
}


//
//	OtWaterPass::renderReflection
//

void OtWaterPass::renderReflection(OtSceneRendererContext& ctx, OtWaterComponent& water) {
	// setup the renderer for the reflection
	auto camera = ctx.camera;
	auto cameraID = ctx.cameraID;
	auto clippingPlane = ctx.clippingPlane;

	ctx.camera = ctx.reflectionCamera;
	ctx.cameraID = OtSceneRendererContext::getReflectionCameraID();
	ctx.clippingPlane = glm::vec4(0.0f, 1.0f, 0.0f, -water.level);

	// render the scene
	backgroundReflectionPass.render(ctx);
	if (ctx.hasOpaqueEntities) { deferredReflectionPass.render(ctx); }
	if (ctx.hasTransparentGeometries) { forwardReflectionPass.render(ctx); }
	if (ctx.hasSkyEntities) { skyReflectionPass.render(ctx); }

	ctx.camera = camera;
	ctx.cameraID = cameraID;
	ctx.clippingPlane = clippingPlane;
}


//
//	OtWaterPass::renderRefraction
//

void OtWaterPass::renderRefraction(OtSceneRendererContext& ctx, OtWaterComponent& water) {
	// setup the renderer for the refraction
	auto camera = ctx.camera;
	auto cameraID = ctx.cameraID;
	auto clippingPlane = ctx.clippingPlane;

	ctx.camera = ctx.refractionCamera;
	ctx.cameraID = OtSceneRendererContext::getRefractionCameraID();
	ctx.clippingPlane = glm::vec4(0.0f, -1.0f, 0.0f, water.level);

	// render the scene
	backgroundRefractionPass.render(ctx);
	if (ctx.hasOpaqueEntities) { deferredRefractionPass.render(ctx); }
	if (ctx.hasTransparentGeometries) { forwardRefractionPass.render(ctx); }

	ctx.camera = camera;
	ctx.cameraID = cameraID;
	ctx.clippingPlane = clippingPlane;
}


//
//	OtWaterPass::renderWater
//

void OtWaterPass::renderWater(OtSceneRendererContext& ctx, OtWaterComponent& water) {
	// setup the rendering pass
	OtRenderPass pass;
	pass.start(framebuffer);
	ctx.pass = &pass;

	// determine wave movement factor
	static float time = 0.0f;
	time += ImGui::GetIO().DeltaTime;
	float moveFactor = std::fmod(water.speed * 0.01f * time, 1.0f);

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
		glm::mat4 inverseProjectionMatrix;
		glm::vec4 waterColor;
		glm::vec2 size;
		float waterLevel;
		float distance;
		float depthFactor;
		float scale;
		float moveFactor;
		float metallic;
		float roughness;
		float ao;
		float reflectivity;
		uint32_t refractanceFlag;
	} fragmentUniforms {
		ctx.camera.viewMatrix,
		ctx.camera.viewProjectionMatrix,
		glm::inverse(ctx.camera.viewProjectionMatrix),
		glm::vec4(water.color, 1.0f),
		glm::vec2(static_cast<float>(framebuffer.getWidth()), static_cast<float>(framebuffer.getHeight())),
		water.level,
		distance,
		water.depthFactor,
		water.scale,
		moveFactor,
		water.metallic,
		water.roughness,
		water.ao,
		water.reflectivity,
		static_cast<uint32_t>(water.useRefractance)
	};

	pass.setFragmentUniforms(0, &fragmentUniforms, sizeof(fragmentUniforms));
	ctx.setLightingUniforms(1, 5);
	ctx.setShadowUniforms(2, 8);

	// bind the textures
	ctx.bindFragmentSampler(0, ctx.waterNormalmapSampler, water.dudv);
	ctx.bindFragmentSampler(1, ctx.waterNormalmapSampler, water.normals);
	pass.bindFragmentSampler(2, ctx.reflectionSampler, reflectionBuffer.getColorTexture());
	pass.bindFragmentSampler(3, ctx.refractionSampler, refractionBuffer.getColorTexture());
	pass.bindFragmentSampler(4, ctx.refractionDepthSampler, refractionBuffer.getDepthTexture());

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

	waterPipeline.setCulling(OtRenderPipeline::Culling::cw);

	waterPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

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
