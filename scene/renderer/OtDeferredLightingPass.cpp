//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
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

	invViewProjUniform.set(0, glm::inverse(ctx.camera.viewProjectionMatrix));
	invViewProjUniform.submit();

	// bind all textures
	ctx.deferedBuffer.bindAlbedoTexture(lightingAlbedoSampler, 0);
	ctx.deferedBuffer.bindNormalTexture(lightingNormalSampler, 1);
	ctx.deferedBuffer.bindPbrTexture(lightingPbrSampler, 2);
	ctx.deferedBuffer.bindEmissiveTexture(lightingEmissiveSampler, 3);
	ctx.deferedBuffer.bindDepthTexture(lightingDepthSampler, 4);

	// run the program
	deferredLightingProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLess);

	pass.runShaderProgram(deferredLightingProgram);
}
