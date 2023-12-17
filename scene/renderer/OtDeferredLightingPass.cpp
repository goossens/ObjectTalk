//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderDeferredLightingPass
//

void OtSceneRenderer::renderDeferredLightingPass(OtSceneRendererContext& ctx) {
	// setup pass
	OtPass pass;
	pass.setFrameBuffer(ctx.compositeBuffer);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

	// submit the uniforms
	submitLightUniforms(ctx.scene, ctx.camera.cameraPosition);

	// bind all textures
	ctx.deferedBuffer.bindAlbedoTexture(deferredLightingAlbedoSampler, 0);
	ctx.deferedBuffer.bindNormalTexture(deferredLightingNormalSampler, 1);
	ctx.deferedBuffer.bindPbrTexture(deferredLightingPbrSampler, 2);
	ctx.deferedBuffer.bindEmissiveTexture(deferredLightingEmissiveSampler, 3);
	ctx.deferedBuffer.bindDepthTexture(deferredLightingDepthSampler, 4);

	// run the program
	deferredLightingProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLess);

	pass.runShaderProgram(deferredLightingProgram);
}
