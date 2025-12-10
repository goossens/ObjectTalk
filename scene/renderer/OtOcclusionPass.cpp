//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRenderPass.h"

#include "OtOcclusionPass.h"

#include "OtSimpleVert.h"
#include "OtSimpleAnimatedVert.h"
#include "OtSimpleInstancingVert.h"
#include "OtSimpleTerrainVert.h"
#include "OtSimpleGrassVert.h"

#include "OtOcclusionOpaqueFrag.h"
#include "OtOcclusionTransparentFrag.h"


//
//	OtOcclusionPass::render
//

void OtOcclusionPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = false;
	}

	// render all entities
	OtRenderPass pass;
	ctx.pass = &pass;
	pass.start(texture);
	renderEntities(ctx);
	pass.end();
}


//
//	OtOcclusionPass::renderOpaqueGeometry
//

void OtOcclusionPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
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
//	OtOcclusionPass::renderOpaqueModel
//

void OtOcclusionPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderModelHelper(
		ctx,
		mrd,
		MaterialSubmission::none,
		opaqueCullingPipeline,
		animatedPipeline);
}


//
//	OtOcclusionPass::renderTerrain
//

void OtOcclusionPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		false,
		terrainCullingPipeline,
		terrainLinesPipeline);
}


//
//	OtOcclusionPass::renderGrass
//

void OtOcclusionPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		grassPipeline);
}


//
//	OtOcclusionPass::renderTransparentGeometry
//

void OtOcclusionPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
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
//	OtOcclusionPass::initializeResources
//

void OtOcclusionPass::initializeResources() {
	opaqueCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	opaqueCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueNoCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	opaqueNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	opaqueLinesPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	opaqueLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueLinesPipeline.setFill(false);

	opaqueInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	opaqueInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	opaqueInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	opaqueInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	opaqueInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedLinesPipeline.setFill(false);

	animatedPipeline.setShaders(OtSimpleAnimatedVert, sizeof(OtSimpleAnimatedVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	animatedPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	animatedPipeline.setVertexDescription(OtVertex::getDescription());
	animatedPipeline.setAnimationDescription(OtVertexBones::getDescription());
	animatedPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainCullingPipeline.setShaders(OtSimpleTerrainVert, sizeof(OtSimpleTerrainVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	terrainCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	terrainCullingPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainLinesPipeline.setShaders(OtSimpleTerrainVert, sizeof(OtSimpleTerrainVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	terrainLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	terrainLinesPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainLinesPipeline.setFill(false);

	grassPipeline.setShaders(OtSimpleGrassVert, sizeof(OtSimpleGrassVert), OtOcclusionOpaqueFrag, sizeof(OtOcclusionOpaqueFrag));
	grassPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	grassPipeline.setVertexDescription(OtVertexPos::getDescription());
	grassPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	grassPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtOcclusionTransparentFrag, sizeof(OtOcclusionTransparentFrag));
	transparentCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentNoCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtOcclusionTransparentFrag, sizeof(OtOcclusionTransparentFrag));
	transparentNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	transparentLinesPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtOcclusionTransparentFrag, sizeof(OtOcclusionTransparentFrag));
	transparentLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentLinesPipeline.setFill(false);

	transparentInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtOcclusionTransparentFrag, sizeof(OtOcclusionTransparentFrag));
	transparentInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtOcclusionTransparentFrag, sizeof(OtOcclusionTransparentFrag));
	transparentInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	transparentInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtOcclusionTransparentFrag, sizeof(OtOcclusionTransparentFrag));
	transparentInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedLinesPipeline.setFill(false);
}
