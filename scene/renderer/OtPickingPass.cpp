//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtRenderPass.h"

#include "OtPickingPass.h"

#include "OtSimpleVert.h"
#include "OtSimpleAnimatedVert.h"
#include "OtSimpleInstancingVert.h"
#include "OtSimpleTerrainVert.h"
#include "OtSimpleGrassVert.h"

#include "OtPickingOpaqueFrag.h"
#include "OtPickingTransparentFrag.h"


//
//	OtPickingPass::render
//

OtEntity OtPickingPass::render(OtSceneRendererContext& ctx, glm::vec2 uv) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// update buffer (if required)
	idBuffer.update(ctx.camera.width / 4, ctx.camera.height / 4);

	// setup camera for identification buffer
	auto camera = ctx.camera;
	ctx.camera = OtCamera(idBuffer.getWidth(), idBuffer.getHeight(), camera.projectionMatrix, camera.viewMatrix);

	// setup pass to render entities as opaque blobs
	OtRenderPass pass;
	ctx.pass = &pass;
	pass.setClearColor(true);
	pass.setClearDepth(true);
	pass.start(idBuffer);

	// render all entity IDs into buffer
	entityMap.clear();
	nextID = 1;
	renderEntities(ctx);

	pass.end();
	ctx.camera = camera;

	// readback entity ID texture to CPU image
	auto& image = idReadback.readback(idBuffer.getColorTexture());

	// find entity ID that appears most often in readback buffer
	std::unordered_map<int, int> frequency;
	int maxCount = 0;
	int picked = 0;
	uint8_t* pixels = (uint8_t*) image.getPixels();
	auto bpp = image.getBpp();
	auto pitch = image.getPitch();

	int px = static_cast<int>(uv.x * idBuffer.getWidth());
	int py = static_cast<int>(uv.y * idBuffer.getHeight());

	for (int y = py - 1; y <= py + 1; y++) {
		for (int x = px - 1; x <= px + 1; x++) {
			int pick = pixels[y * pitch + x * bpp];

			if (pick) {
				int count = 1;

				if (frequency.find(pick) != frequency.end()) {
					count = frequency[pick] + 1;
				}

				frequency[pick] = count;

				if (count > maxCount) {
					maxCount = count;
					picked = pick;
				}
			}
		}
	}

	// if something was hit, report it back
	return maxCount ? entityMap[picked] : OtEntityNull;
}


//
//	OtPickingPass::renderOpaqueGeometry
//

void OtPickingPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	setFragmentUniforms(ctx, grd.entity);

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
//	OtPickingPass::renderOpaqueModel
//

void OtPickingPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	setFragmentUniforms(ctx, mrd.entity);

	renderModelHelper(
		ctx,
		mrd,
		MaterialSubmission::none,
		opaqueCullingPipeline,
		animatedPipeline);
}


//
//	OtPickingPass::renderTerrain
//

void OtPickingPass::renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain) {
	setFragmentUniforms(ctx, entity);

	renderTerrainHelper(
		ctx,
		terrain,
		false,
		terrainCullingPipeline,
		terrainLinesPipeline);
}


//
//	OtPickingPass::renderGrass
//

void OtPickingPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	setFragmentUniforms(ctx, entity);

	renderGrassHelper(
		ctx,
		entity,
		grass,
		grassPipeline);
}


//
//	OtPickingPass::renderTransparentGeometry
//

void OtPickingPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	setFragmentUniforms(ctx, grd.entity);
	albedoUniformSlot = 1;

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
//	OtPickingPass::setFragmentUniforms
//

void OtPickingPass::setFragmentUniforms(OtSceneRendererContext& ctx, OtEntity entity) {
	struct Uniforms {
		float entityID;
	} uniforms {
		static_cast<float>(nextID) / 255.0f
	};

	ctx.pass->setFragmentUniforms(0, &uniforms, sizeof(uniforms));
	entityMap[nextID++] = entity;
}


//
//	OtPickingPass::initializeResources
//

void OtPickingPass::initializeResources() {
	opaqueCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	opaqueCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	opaqueCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueNoCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	opaqueNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	opaqueNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	opaqueLinesPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	opaqueLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	opaqueLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueLinesPipeline.setFill(false);

	opaqueInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	opaqueInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	opaqueInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	opaqueInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	opaqueInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	opaqueInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	opaqueInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	opaqueInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	opaqueInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	opaqueInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	opaqueInstancedLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	opaqueInstancedLinesPipeline.setFill(false);

	animatedPipeline.setShaders(OtSimpleAnimatedVert, sizeof(OtSimpleAnimatedVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	animatedPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	animatedPipeline.setVertexDescription(OtVertex::getDescription());
	animatedPipeline.setAnimatedDescription(OtVertexBones::getDescription());
	animatedPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	animatedPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainCullingPipeline.setShaders(OtSimpleTerrainVert, sizeof(OtSimpleTerrainVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	terrainCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	terrainCullingPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	terrainLinesPipeline.setShaders(OtSimpleTerrainVert, sizeof(OtSimpleTerrainVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	terrainLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	terrainLinesPipeline.setVertexDescription(OtVertexPos::getDescription());
	terrainLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	terrainLinesPipeline.setFill(false);

	grassPipeline.setShaders(OtSimpleGrassVert, sizeof(OtSimpleGrassVert), OtPickingOpaqueFrag, sizeof(OtPickingOpaqueFrag));
	grassPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	grassPipeline.setVertexDescription(OtVertexPos::getDescription());
	grassPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	grassPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtPickingTransparentFrag, sizeof(OtPickingTransparentFrag));
	transparentCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	transparentCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentNoCullingPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtPickingTransparentFrag, sizeof(OtPickingTransparentFrag));
	transparentNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	transparentNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	transparentLinesPipeline.setShaders(OtSimpleVert, sizeof(OtSimpleVert), OtPickingTransparentFrag, sizeof(OtPickingTransparentFrag));
	transparentLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	transparentLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentLinesPipeline.setFill(false);

	transparentInstancedCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtPickingTransparentFrag, sizeof(OtPickingTransparentFrag));
	transparentInstancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	transparentInstancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentInstancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	transparentInstancedNoCullingPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtPickingTransparentFrag, sizeof(OtPickingTransparentFrag));
	transparentInstancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	transparentInstancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	transparentInstancedLinesPipeline.setShaders(OtSimpleInstancingVert, sizeof(OtSimpleInstancingVert), OtPickingTransparentFrag, sizeof(OtPickingTransparentFrag));
	transparentInstancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::r8d32);
	transparentInstancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	transparentInstancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	transparentInstancedLinesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	transparentInstancedLinesPipeline.setFill(false);
}
