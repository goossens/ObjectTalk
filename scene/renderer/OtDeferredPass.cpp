//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"

#include "OtRenderPass.h"
#include "OtVertex.h"

#include "OtDeferredPass.h"
#include "OtShaders.h"


//
//	OtDeferredPass::render
//

void OtDeferredPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// run the geometry and lighting passes
	renderGeometry(ctx);
	renderDirectionalLight(ctx);

	if (ctx.hasPointLighting) {
		renderPointLights(ctx);
	}
}


//
//	OtDeferredPass::renderGeometry
//

void OtDeferredPass::renderGeometry(OtSceneRendererContext& ctx) {
	// setup pass
	OtRenderPass pass;
	pass.setClearColor(true, glm::vec4(0.0f));
	pass.setClearDepth(true);
	pass.start(gbuffer);
	ctx.pass = &pass;

	// submit common fragment uniforms
	// uniform slot 0 and sampler slots 0-4 are used for materials
	ctx.setClippingUniforms(1);

	// render all entities
	renderEntities(ctx);

	// we're done
	pass.end();
}


//
//	OtDeferredPass::renderOpaqueGeometry
//

void OtDeferredPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderGeometryHelper(
		ctx,
		grd,
		MaterialSubmission::full,
		cullingPipeline,
		noCullingPipeline,
		linesPipeline,
		instancedCullingPipeline,
		instancedNoCullingPipeline,
		instancedLinesPipeline);
}


//
//	OtDeferredPass::renderOpaqueModel
//

void OtDeferredPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderModelHelper(
		ctx,
		mrd,
		MaterialSubmission::full,
		cullingPipeline,
		animatedPipeline);
}


//
//	OtDeferredPass::renderTerrain
//

void OtDeferredPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		true,
		terrainCullingPipeline,
		terrainLinesPipeline);
}


//
//	OtDeferredPass::renderGrass
//

void OtDeferredPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		grassPipeline);
}


//
//	OtDeferredPass::renderDirectionalLight
//

void OtDeferredPass::renderDirectionalLight(OtSceneRendererContext& ctx) {
	// setup pass
	OtRenderPass pass;
	pass.start(framebuffer);
	pass.bindPipeline(directionalLightPipeline);
	ctx.pass = &pass;

	// set uniforms
	struct Uniforms {
		glm::mat4 viewUniform;
		glm::mat4 invViewProjUniform;
	} uniforms {
		ctx.camera.viewMatrix,
		glm::inverse(ctx.camera.viewProjectionMatrix)
	};

	pass.bindFragmentUniforms(0, &uniforms, sizeof(uniforms));
	ctx.setLightingUniforms(1, 5);
	ctx.setShadowUniforms(2, 8);

	// bind samplers
	pass.bindFragmentSampler(0, ctx.lightingAlbedoSampler, gbuffer.getAlbedoTexture());
	pass.bindFragmentSampler(1, ctx.lightingNormalSampler, gbuffer.getNormalTexture());
	pass.bindFragmentSampler(2, ctx.lightingPbrSampler, gbuffer.getPbrTexture());
	pass.bindFragmentSampler(3, ctx.lightingEmissiveSampler, gbuffer.getEmissiveTexture());
	pass.bindFragmentSampler(4, ctx.lightingDepthSampler, gbuffer.getDepthTexture());

	pass.render(3);
	pass.end();
}


//
//	OtDeferredPass::renderPointLights
//

void OtDeferredPass::renderPointLights(OtSceneRendererContext& ctx) {
	std::vector<OtVertexPointLight> lights;

	for (auto&& [entity, component] : ctx.scene->view<OtPointLightComponent>().each()) {
		glm::mat4 transform = ctx.scene->getGlobalTransform(entity);

		glm::vec3 translate{0.0f};
		glm::quat rotate;
		glm::vec3 scale{0.0f};
		glm::vec3 skew{0.0f};
		glm::vec4 perspective{0.0f};
		glm::decompose(transform, scale, rotate, translate, skew, perspective);

		lights.emplace_back(scale * component.offset + translate, component.color, component.radius);
	}

	// setup pass
	OtRenderPass pass;
	pass.start(framebuffer);
	pass.bindPipeline(pointLightsPipeline);
	ctx.pass = &pass;

	// set vertex uniforms
	struct VertexUniforms {
		glm::mat4 viewProjectionMatrix;
	} vertexUniforms{
		ctx.camera.viewProjectionMatrix
	};

	pass.bindVertexUniforms(0, &vertexUniforms, sizeof(vertexUniforms));

	// set fragment uniforms
	struct FragmentUniforms {
		glm::mat4 inverseViewProjectionMatrix;
		float viewWidth;
		float viewHeight;
	} fragmentUniforms{
		glm::inverse(ctx.camera.viewProjectionMatrix),
		static_cast<float>(ctx.camera.width),
		static_cast<float>(ctx.camera.height)
	};

	pass.bindFragmentUniforms(0, &fragmentUniforms, sizeof(fragmentUniforms));
	ctx.setLightingUniforms(1, 4);

	// send out particle(instance) data
	pointLightInstances.set(lights.data(), lights.size(), OtVertexParticle::getDescription());
	pass.setInstanceData(pointLightInstances);

	// bind samplers
	pass.bindFragmentSampler(0, ctx.lightingAlbedoSampler, gbuffer.getAlbedoTexture());
	pass.bindFragmentSampler(1, ctx.lightingNormalSampler, gbuffer.getNormalTexture());
	pass.bindFragmentSampler(2, ctx.lightingPbrSampler, gbuffer.getPbrTexture());
	pass.bindFragmentSampler(3, ctx.lightingDepthSampler, gbuffer.getDepthTexture());

	// render lights
	pass.render(pointLightVertices, pointLightIndices);
	pass.end();
}


//
//	OtDeferredPass::initializeResources
//

void OtDeferredPass::initializeResources() {
	// configure rendering pipelines
	cullingPipeline.setShaders(OtDeferredVert, OtDeferredVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	cullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	cullingPipeline.setVertexDescription(OtVertex::getDescription());
	cullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	cullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	noCullingPipeline.setShaders(OtDeferredVert, OtDeferredVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	noCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	noCullingPipeline.setVertexDescription(OtVertex::getDescription());
	noCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	linesPipeline.setShaders(OtDeferredVert, OtDeferredVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	linesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	linesPipeline.setVertexDescription(OtVertex::getDescription());
	linesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	linesPipeline.setFill(false);

	instancedCullingPipeline.setShaders(OtDeferredInstancingVert, OtDeferredInstancingVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	instancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	instancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	instancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	instancedCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	instancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	instancedNoCullingPipeline.setShaders(OtDeferredInstancingVert, OtDeferredInstancingVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	instancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	instancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	instancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	instancedNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	instancedLinesPipeline.setShaders(OtDeferredInstancingVert, OtDeferredInstancingVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	instancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	instancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	instancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	instancedLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	instancedLinesPipeline.setFill(false);

	animatedPipeline.setShaders(OtDeferredAnimatedVert, OtDeferredAnimatedVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	animatedPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	animatedPipeline.setVertexDescription(OtVertex::getDescription());
	animatedPipeline.setAnimationDescription(OtVertexBones::getDescription());
	animatedPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	animatedPipeline.setCulling(OtRenderPipeline::Culling::cw);

	cullingPipeline.setShaders(OtDeferredVert, OtDeferredVertSize, OtDeferredPbrFrag, OtDeferredPbrFragSize);
	cullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	cullingPipeline.setVertexDescription(OtVertex::getDescription());
	cullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	cullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainCullingPipeline.setShaders(OtTerrainVert, OtTerrainVertSize, OtTerrainFrag, OtTerrainFragSize);
	terrainCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	terrainCullingPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	terrainLinesPipeline.setShaders(OtTerrainVert, OtTerrainVertSize, OtTerrainFrag, OtTerrainFragSize);
	terrainLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	terrainLinesPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainLinesPipeline.setFill(false);

	grassPipeline.setShaders(OtGrassVert, OtGrassVertSize, OtGrassFrag, OtGrassFragSize);
	grassPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::gBuffer);
	grassPipeline.setVertexDescription(OtVertexPos::getDescription());
	grassPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	grassPipeline.setCulling(OtRenderPipeline::Culling::cw);

	directionalLightPipeline.setShaders(OtFullScreenVert, OtFullScreenVertSize, OtDeferredLightingFrag, OtDeferredLightingFragSize);
	directionalLightPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	directionalLightPipeline.setDepthTest(OtRenderPipeline::CompareOperation::always);

	pointLightsPipeline.setShaders(OtPointLightsVert, OtPointLightsVertSize, OtPointLightsFrag, OtPointLightsFragSize);
	pointLightsPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	pointLightsPipeline.setVertexDescription(OtVertexPos::getDescription());
	pointLightsPipeline.setInstanceDescription(OtVertexPointLight::getDescription());
	pointLightsPipeline.setDepthTest(OtRenderPipeline::CompareOperation::greaterEqual);
	pointLightsPipeline.setCulling(OtRenderPipeline::Culling::ccw);

	pointLightsPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::one,
		OtRenderPipeline::BlendFactor::one
	);

	static glm::vec3 vertices[] = {
		glm::vec3{-1.0f, -1.0f, 1.0f}, glm::vec3{ 1.0f, -1.0f, 1.0f}, glm::vec3{-1.0f, 1.0f, 1.0f}, glm::vec3{1.0f, 1.0f, 1.0f},
		glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{ 1.0f, -1.0f, -1.0f}, glm::vec3{-1.0f, 1.0f, -1.0f}, glm::vec3{1.0f, 1.0f, -1.0f}
	};

	static uint32_t indices[] = {
		0, 1, 3, 3, 2, 0, // front
		5, 4, 6, 6, 7, 5, // back
		4, 0, 2, 2, 6, 4, // left
		1, 5, 7, 7, 3, 1, // right
		2, 3, 7, 7, 6, 2, // top
		4, 5, 1, 1, 0, 4  // bottom
	};

	pointLightVertices.set(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPos::getDescription());
	pointLightIndices.set(indices, sizeof(indices) / sizeof(*indices));
}
