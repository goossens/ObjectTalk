//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPass.h"

#include "OtOcclusionPass.h"


//
//	OtOcclusionPass::render
//

void OtOcclusionPass::render(OtSceneRendererContext& ctx) {
	// setup the rendering pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
	pass.setFrameBuffer(framebuffer);

	// render all entities
	renderEntities(ctx, pass);
}


//
//	OtOcclusionPass::renderOpaqueGeometry
//

void OtOcclusionPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderOpaqueGeometryHelper(
		ctx,
		grd,
		OtStateWriteRgb | OtStateLines,
		OtStateWriteRgb | (grd.component->cullBack ? OtStateCullCw : 0),
		MaterialSubmission::none,
		opaqueProgram,
		instancedOpaqueProgram);
}


//
//	OtOcclusionPass::renderOpaqueModel
//

void OtOcclusionPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderOpaqueModelHelper(
		ctx,
		mrd,
		OtStateWriteRgb | OtStateCullCw,
		MaterialSubmission::none,
		animatedOpaqueProgram,
		opaqueProgram);
}


//
//	OtOcclusionPass::renderTerrain
//

void OtOcclusionPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		OtStateWriteRgb | OtStateLines,
		OtStateWriteRgb | OtStateCullCw,
		terrainProgram);
}


//
//	OtOcclusionPass::renderGrass
//

void OtOcclusionPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		OtStateWriteRgb | OtStateCullCw,
		grassProgram);
}


//
//	OtOcclusionPass::renderTransparentGeometry
//

void OtOcclusionPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderTransparentGeometryHelper(
		ctx,
		grd,
		OtStateWriteRgb | OtStateLines,
		OtStateWriteRgb | (grd.component->cullBack ? OtStateCullCw : 0),
		MaterialSubmission::justAlbedo,
		transparentProgram,
		instancedTransparentProgram);
}
