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

void OtSceneRenderer::renderDeferredLightingPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setFrameBuffer(compositeBuffer);
	pass.submitQuad(width, height);

	// submit the uniforms
	submitLightUniforms(scene);

	inverseTransform.set(0, glm::inverse(viewProjectionMatrix));
	inverseTransform.submit();

	// bind all textures
	gbuffer.bindAlbedoTexture(deferredLightingAlbedoSampler, 0);
	gbuffer.bindNormalTexture(deferredLightingNormalSampler, 1);
	gbuffer.bindPbrTexture(deferredLightingPbrSampler, 2);
	gbuffer.bindEmissiveTexture(deferredLightingEmissiveSampler, 3);
	gbuffer.bindDepthTexture(deferredLightingDepthSampler, 4);

	// run the program
	deferredLightingProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShaderProgram(deferredLightingProgram);
}
