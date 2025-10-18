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
	// update shadowmaps
	ctx.csm->update(ctx.camera, ctx.directionalLightDirection);

	// save context part the we will temporarily overwrite
	auto camera = ctx.camera;
	auto renderingShadow = ctx.renderingShadow;

	// render each cascade
	for (size_t i = 0; i < OtCascadedShadowMap::maxCascades; i++) {
		// setup pass to render entities as opaque blobs
		OtPass pass;
		pass.setRectangle(0, 0, ctx.csm->getSize(), ctx.csm->getSize());
		pass.setFrameBuffer(ctx.csm->getFrameBuffer(i));
		pass.setClear(false, true);
		pass.setTransform(ctx.csm->getCamera(i).viewMatrix, ctx.csm->getCamera(i).projectionMatrix);
		pass.touch();

		ctx.camera = ctx.csm->getCamera(i);
		ctx.renderingShadow = true;

		// render all entities
		renderEntities(ctx, pass);
	}

	// restore old rendering context
	ctx.camera = camera;
	ctx.renderingShadow = renderingShadow;
}


//
//	OtShadowPass::renderOpaqueGeometry
//

void OtShadowPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry) {
	renderOpaqueGeometryHelper(
		ctx,
		entity,
		geometry,
		OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteZ | OtStateDepthTestLess | (geometry.cullBack ? OtStateCullCw : 0),
		opaqueProgram,
		instancedOpaqueProgram);
}


//
//	OtShadowPass::renderOpaqueModel
//

void OtShadowPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtEntity entity, OtModelComponent& model) {
	renderOpaqueModelHelper(
		ctx,
		entity,
		model,
		OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
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
		OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
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
		OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		grassProgram);

}


//
//	OtShadowPass::renderTransparentGeometry
//

void OtShadowPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry) {
	renderTransparentGeometryHelper(
		ctx,
		entity,
		geometry,
		OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		transparentProgram,
		instancedTransparentProgram);
}
