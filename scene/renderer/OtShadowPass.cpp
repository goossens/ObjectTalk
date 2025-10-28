//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtShadowPass.h"


//
//	OtShadowPass::render
//

void OtShadowPass::render(OtSceneRendererContext& ctx) {
	// save context part the we will temporarily overwrite
	auto camera = ctx.camera;
	auto cameraID = ctx.cameraID;
	auto renderingShadow = ctx.renderingShadow;
	ctx.renderingShadow = true;

	// render each cascade
	for (size_t i = 0; i < OtCascadedShadowMap::maxCascades; i++) {
		// setup pass to render entities as opaque blobs
		OtPass pass;
		pass.setRectangle(0, 0, ctx.csm.getSize(), ctx.csm.getSize());
		pass.setFrameBuffer(ctx.csm.getFrameBuffer(i));
		pass.setClear(false, true);
		pass.setTransform(ctx.csm.getCamera(i).viewMatrix, ctx.csm.getCamera(i).projectionMatrix);
		pass.touch();

		// render all entities
		ctx.camera = ctx.csm.getCamera(i);
		ctx.cameraID = OtSceneRendererContext::getShadowCameraID(i);
		renderEntities(ctx, pass);
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
	renderOpaqueGeometryHelper(
		ctx,
		grd,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateLines,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | (grd.component->cullBack ? OtPass::stateCullCw : 0),
		MaterialSubmission::none,
		opaqueProgram,
		instancedOpaqueProgram);
}


//
//	OtShadowPass::renderOpaqueModel
//

void OtShadowPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderOpaqueModelHelper(
		ctx,
		mrd,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateCullCw,
		MaterialSubmission::none,
		animatedOpaqueProgram,
		opaqueProgram);
}


//
//	OtShadowPass::renderTerrain
//

void OtShadowPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateLines,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateCullCw,
		terrainProgram);
}


//
//	OtShadowPass::renderGrass
//

void OtShadowPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateCullCw,
		grassProgram);

}


//
//	OtShadowPass::renderTransparentGeometry
//

void OtShadowPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderTransparentGeometryHelper(
		ctx,
		grd,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateLines,
		OtPass::stateWriteZ | OtPass::stateDepthTestLess | OtPass::stateCullCw,
		MaterialSubmission::justAlbedo,
		transparentProgram,
		instancedTransparentProgram);
}
