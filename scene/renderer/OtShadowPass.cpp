//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRenderPass.h"

#include "OtShadowPass.h"
#include "OtShaders.h"


//
//	OtShadowPass::render
//

void OtShadowPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// save context part the we will temporarily overwrite
	auto camera = ctx.camera;
	auto cameraID = ctx.cameraID;
	auto renderingShadow = ctx.renderingShadow;

	// put context in shadow rendering mode
	ctx.renderingShadow = true;

	// render each cascade
	for (size_t i = 0; i < OtCascadedShadowMap::maxCascades; i++) {
		// setup pass to render entities as opaque blobs
		OtRenderPass pass;
		pass.setClearDepth(true);
		pass.start(ctx.csm.getFrameBuffer(i));
		ctx.pass = &pass;

		ctx.camera = ctx.csm.getCamera(i);
		ctx.cameraID = OtSceneRendererContext::getShadowCameraID(i);

		// render all entities
		renderEntities(ctx);

		// we're done
		pass.end();
	}

	// restore old rendering context
	ctx.camera = camera;
	ctx.cameraID = cameraID;
	ctx.renderingShadow = renderingShadow;
}


//
//	OtShadowPass::renderOpaqueGeometry
//

void OtShadowPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderGeometryHelper(
		ctx,
		grd,
		MaterialSubmission::none,
		opaqueCullingPipeline,
		opaqueNoCullingPipeline,
		opaqueLinesPipeline,
		opaqueInstancedCullingPipeline,
		opaqueInstancedNoCullingPipeline,
		opaqueInstancedLinesPipeline);
}


//
//	OtShadowPass::renderOpaqueModel
//

void OtShadowPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderModelHelper(
		ctx,
		mrd,
		MaterialSubmission::none,
		opaqueCullingPipeline,
		animatedPipeline);
}


//
//	OtShadowPass::renderTerrain
//

void OtShadowPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		false,
		terrainCullingPipeline,
		terrainLinesPipeline);
}


//
//	OtShadowPass::renderGrass
//

void OtShadowPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		grassPipeline);
}


//
//	OtShadowPass::renderTransparentGeometry
//

void OtShadowPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderGeometryHelper(
		ctx,
		grd,
		MaterialSubmission::justAlbedo,
		transparentCullingPipeline,
		transparentNoCullingPipeline,
		transparentLinesPipeline,
		transparentInstancedCullingPipeline,
		transparentInstancedNoCullingPipeline,
		transparentInstancedLinesPipeline);
}


//
//	OtShadowPass::initializeResources
//

void OtShadowPass::initializeResources() {
	opaqueCullingPipeline.setShaders(OtSimpleVert, OtSimpleVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	opaqueCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	opaqueCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueNoCullingPipeline.setShaders(OtSimpleVert, OtSimpleVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	opaqueNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	opaqueNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	opaqueLinesPipeline.setShaders(OtSimpleVert, OtSimpleVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	opaqueLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	opaqueLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueLinesPipeline.setFill(false);

	opaqueInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, OtSimpleInstancingVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	opaqueInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	opaqueInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, OtSimpleInstancingVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	opaqueInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	opaqueInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	opaqueInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, OtSimpleInstancingVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	opaqueInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	opaqueInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueInstancedLinesPipeline.setFill(false);

	animatedPipeline.setShaders(OtSimpleAnimatedVert, OtSimpleAnimatedVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	animatedPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	animatedPipeline.setVertexDescription(OtVertex::getDescription());
	animatedPipeline.setAnimationDescription(OtVertexBones::getDescription());
	animatedPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	animatedPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainCullingPipeline.setShaders(OtSimpleTerrainVert, OtSimpleTerrainVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	terrainCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	terrainCullingPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainLinesPipeline.setShaders(OtSimpleTerrainVert, OtSimpleTerrainVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	terrainLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	terrainLinesPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainLinesPipeline.setFill(false);

	grassPipeline.setShaders(OtSimpleGrassVert, OtSimpleGrassVertSize, OtShadowOpaqueFrag, OtShadowOpaqueFragSize);
	grassPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	grassPipeline.setVertexDescription(OtVertexPos::getDescription());
	grassPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	grassPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentCullingPipeline.setShaders(OtSimpleVert, OtSimpleVertSize, OtShadowTransparentFrag, OtShadowTransparentFragSize);
	transparentCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	transparentCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentNoCullingPipeline.setShaders(OtSimpleVert, OtSimpleVertSize, OtShadowTransparentFrag, OtShadowTransparentFragSize);
	transparentNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	transparentNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	transparentLinesPipeline.setShaders(OtSimpleVert, OtSimpleVertSize, OtShadowTransparentFrag, OtShadowTransparentFragSize);
	transparentLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	transparentLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentLinesPipeline.setFill(false);

	transparentInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, OtSimpleInstancingVertSize, OtShadowTransparentFrag, OtShadowTransparentFragSize);
	transparentInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	transparentInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, OtSimpleInstancingVertSize, OtShadowTransparentFrag, OtShadowTransparentFragSize);
	transparentInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	transparentInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	transparentInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, OtSimpleInstancingVertSize, OtShadowTransparentFrag, OtShadowTransparentFragSize);
	transparentInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::d32);
	transparentInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentInstancedLinesPipeline.setFill(false);
}
