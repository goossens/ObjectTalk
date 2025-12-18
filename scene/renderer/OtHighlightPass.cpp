//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRenderPass.h"

#include "OtHighlightPass.h"

#include "OtSimpleVert.h"
#include "OtSimpleAnimatedVert.h"
#include "OtSimpleInstancingVert.h"
#include "OtSimpleTerrainVert.h"
#include "OtSimpleGrassVert.h"

#include "OtSelectOpaqueFrag.h"
#include "OtSelectTransparentFrag.h"

#include "OtFullScreenVert.h"
#include "OtOutlineFrag.h"


//
//	OtHighlightPass::render
//

void OtHighlightPass::render(OtSceneRendererContext& ctx, OtTexture* texture, OtEntity entity) {
	// see if we have a "highlightable" entity
	if (isHighlightable(ctx.scene, entity)) {
		// initialize resources (if required)
		if (!resourcesInitialized) {
			initializeResources();
			resourcesInitialized = false;
		}

	// run both passes
		renderSelectedPass(ctx, entity);
		renderHighlightPass(ctx, texture);
	}
}


//
//	OtHighlightPass::renderSelectedPass
//

void OtHighlightPass::renderSelectedPass(OtSceneRendererContext& ctx, OtEntity entity) {
	// update framebuffer size
	selectedBuffer.update(ctx.camera.width, ctx.camera.height);

	// setup pass to render selected entities as opaque blobs
	OtRenderPass pass;
	ctx.pass = &pass;
	pass.setClearColor(true);
	pass.setClearDepth(true);
	pass.start(selectedBuffer);
	renderHighlight(ctx, entity);
	pass.end();
}


//
//	OtHighlightPass::renderHighlightPass
//

void OtHighlightPass::renderHighlightPass(OtSceneRendererContext& ctx, OtTexture* texture) {
		// configure pass
		OtRenderPass pass;
		pass.start(*texture);
		pass.bindPipeline(outlinePipeline);
		pass.bindFragmentSampler(0, ctx.highlightSampler, selectedBuffer.getColorTexture());

		// set uniforms
		struct Uniforms {
			glm::vec2 texelSize;
		} uniforms {
			glm::vec2(1.0f / static_cast<float>(ctx.camera.width), 1.0f / static_cast<float>(ctx.camera.height))
		};

		pass.bindFragmentUniforms(0, &uniforms, sizeof(uniforms));
		pass.render(3);
		pass.end();
}


//
//	OtHighlightPass::renderHighlight
//

void OtHighlightPass::renderHighlight(OtSceneRendererContext& ctx, OtEntity entity) {
	// render entity and its children
	renderEntity(ctx, entity);
	OtEntity child = ctx.scene->getFirstChild(entity);

	while (ctx.scene->isValidEntity(child)) {
		renderHighlight(ctx, child);
		child = ctx.scene->getNextSibling(child);
	}
}


//
//	OtHighlightPass::isHighlightable
//

bool OtHighlightPass::isHighlightable(OtScene* scene, OtEntity entity) {
	// see if specified entity or one of the children is "highlightable"
	bool highlightable = false;

	if (scene->isValidEntity(entity)) {
		highlightable |= scene->hasComponent<OtGeometryComponent>(entity);
		highlightable |= scene->hasComponent<OtModelComponent>(entity);
		highlightable |= scene->hasComponent<OtTerrainComponent>(entity);
		highlightable |= scene->hasComponent<OtGrassComponent>(entity);

		OtEntity child = scene->getFirstChild(entity);

		while (scene->isValidEntity(child)) {
			highlightable |= isHighlightable(scene, child);
			child = scene->getNextSibling(child);
		}
	}

	return highlightable;
}


//
//	OtHighlightPass::renderOpaqueGeometry
//

void OtHighlightPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
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
//	OtHighlightPass::renderOpaqueModel
//

void OtHighlightPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderModelHelper(
		ctx,
		mrd,
		MaterialSubmission::none,
		opaqueCullingPipeline,
		animatedPipeline);
}


//
//	OtHighlightPass::renderTerrain
//

void OtHighlightPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		false,
		terrainCullingPipeline,
		terrainLinesPipeline);
}


//
//	OtHighlightPass::renderGrass
//

void OtHighlightPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		grassPipeline);
}


//
//	OtHighlightPass::renderTransparentGeometry
//

void OtHighlightPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
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
//	OtHighlightPass::initializeResources
//

void OtHighlightPass::initializeResources() {
	opaqueCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	opaqueCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueNoCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	opaqueNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	opaqueLinesPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	opaqueLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueLinesPipeline.setFill(false);

	opaqueInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	opaqueInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	opaqueInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	opaqueInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	opaqueInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	opaqueInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedLinesPipeline.setFill(false);

	animatedPipeline.setShaders(OtSimpleAnimatedVert, sizeof(OtSimpleAnimatedVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	animatedPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	animatedPipeline.setVertexDescription(OtVertex::getDescription());
	animatedPipeline.setAnimationDescription(OtVertexBones::getDescription());
	animatedPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainCullingPipeline.setShaders(OtSimpleTerrainVert, sizeof(OtSimpleTerrainVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	terrainCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	terrainCullingPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainLinesPipeline.setShaders(OtSimpleTerrainVert, sizeof(OtSimpleTerrainVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	terrainLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	terrainLinesPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainLinesPipeline.setFill(false);

	grassPipeline.setShaders(OtSimpleGrassVert, sizeof(OtSimpleGrassVert), OtSelectOpaqueFrag, sizeof(OtSelectOpaqueFrag));
	grassPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	grassPipeline.setVertexDescription(OtVertexPos::getDescription());
	grassPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	grassPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtSelectTransparentFrag, sizeof(OtSelectTransparentFrag));
	transparentCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentNoCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtSelectTransparentFrag, sizeof(OtSelectTransparentFrag));
	transparentNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	transparentLinesPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtSelectTransparentFrag, sizeof(OtSelectTransparentFrag));
	transparentLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentLinesPipeline.setFill(false);

	transparentInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtSelectTransparentFrag, sizeof(OtSelectTransparentFrag));
	transparentInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtSelectTransparentFrag, sizeof(OtSelectTransparentFrag));
	transparentInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());

	transparentInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtSelectTransparentFrag, sizeof(OtSelectTransparentFrag));
	transparentInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8);
	transparentInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedLinesPipeline.setFill(false);

	outlinePipeline.setShaders(OtFullScreenVert, sizeof(OtFullScreenVert), OtOutlineFrag, sizeof(OtOutlineFrag));
	outlinePipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16);

	outlinePipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);
}
