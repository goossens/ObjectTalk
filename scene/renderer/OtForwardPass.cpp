//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "imgui.h"

#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderForwardGeometryPass
//

void OtSceneRenderer::renderForwardGeometryPass(OtSceneRendererContext& ctx) {
	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(ctx.compositeBuffer);
	pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

	// render all water entities
	if (ctx.hasWaterEntities) {
		ctx.scene->view<OtWaterComponent>().each([&](auto entity, auto& water) {
			renderForwardWater(ctx, pass, water);
		});
	}

	// render all transparent geometries
	ctx.scene->view<OtGeometryComponent, OtMaterialComponent>().each([&](auto entity, auto& geometry, auto& material) {
		if (geometry.transparent && geometry.asset.isReady()) {
			renderForwardGeometry(ctx, pass, entity, geometry, material);
		}
	});
}


//
//	OtSceneRenderer::renderForwardWater
//

void OtSceneRenderer::renderForwardWater(OtSceneRendererContext& ctx, OtPass& pass, OtWaterComponent& water) {
	// send out geometry
	static glm::vec3 vertices[] = {
		glm::vec3{-1.0, -1.0, 0.0},
		glm::vec3{1.0, -1.0, 0.0},
		glm::vec3{1.0, 1.0, 0.0},
		glm::vec3{-1.0, 1.0, 0.0}
	};

	OtTransientVertexBuffer vertexBuffer;
	vertexBuffer.submit(vertices, 4, OtVertexPos::getLayout());

	static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
	OtTransientIndexBuffer indexBuffer;
	indexBuffer.submit(indices, 6);

	// determine time
	float time = getRunningTime() * 0.1f * water.speed;

	// get maximum distance in clip space
	glm::vec4 farPoint = ctx.camera.projectionMatrix * glm::vec4(0.0, water.level, -water.distance, 1.0);
	float distance = farPoint.z / farPoint.w;

	// submit water and light uniforms
	waterUniforms.setValue(0, water.level, distance, 0.0f, 0.0f);
	waterUniforms.setValue(1, water.scale, getTextureAssetWidth(water.normals), time, 0.0f);
	waterUniforms.setValue(2, water.metallic, water.roughness, water.ao, water.reflectivity);
	waterUniforms.setValue(3, water.color, float(water.useRefractance));
	waterUniforms.submit();

	submitLightUniforms(ctx);

	// bind the textures
	submitTextureSampler(normalmapSampler, 0, water.normals);
	reflectionBuffer.bindColorTexture(reflectionSampler, 1);
	refractionBuffer.bindColorTexture(refractionSampler, 2);
	refractionBuffer.bindDepthTexture(refractionDepthSampler, 3);

	// run the program
	forwardWaterProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLess |
		OtStateBlendAlpha);

	pass.runShaderProgram(forwardWaterProgram);
}


//
//	OtSceneRenderer::renderForwardGeometry
//

void OtSceneRenderer::renderForwardGeometry(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program;

	// get camera frustum and geometry AABB
	auto& frustum = ctx.camera.frustum;
	auto& aabb =  geometry.asset->getGeometry().getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(frustum, aabb)) {
			visible = true;
			program = &forwardInstancingProgram;
		}

	} else {
		// see if geometry is visible
		if (frustum.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = &forwardPbrProgram;
		}
	}

	// ensure geometry is visible
	if (visible) {
		// submit the material, clipping and light uniforms
		submitMaterialUniforms(*material.material);
		submitClippingUniforms(ctx.clippingPlane);
		submitLightUniforms(ctx);

		// submit the geometry
		auto geom = geometry.asset->getGeometry();

		if (geometry.wireframe) {
			geom.submitLines();

		} else {
			geom.submitTriangles();
		}

		// set the program state
		if (geometry.wireframe) {
			program->setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateLines |
				OtStateBlendAlpha);

		} else if (geometry.cullback) {
			program->setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateCullCw |
				OtStateBlendAlpha);

		} else {
			program->setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateBlendAlpha);
		}

		// set the transform
		program->setTransform(ctx.scene->getGlobalTransform(entity));

		// run the program
		pass.runShaderProgram(*program);
	}
}
