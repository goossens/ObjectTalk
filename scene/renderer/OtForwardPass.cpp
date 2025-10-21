//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtForwardPass.h"


//
//	OtForwardPass::render
//

void OtForwardPass::render(OtSceneRendererContext& ctx) {
	// setup the rendering pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
	pass.touch();

	// render all entities
	renderEntities(ctx, pass);
}


//
//	OtForwardPass::renderTransparentGeometry
//

void OtForwardPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	ctx.submitLightingUniforms();
	ctx.submitShadowUniforms();

	renderTransparentGeometryHelper(
		ctx,
		grd,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateBlendAlpha | OtStateLines,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateBlendAlpha | (grd.component->cullBack ? OtStateCullCw : 0),
		MaterialSubmission::full,
		transparentProgram,
		instancedTransparentProgram);
}
