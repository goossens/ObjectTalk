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
//	OtSceneRenderer::renderLightingPass
//

void OtSceneRenderer::renderLightingPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setFrameBuffer(compositeBuffer);
	pass.submitQuad(width, height);

	// submit the uniforms
	submitLightUniforms(scene);

	inverseTransform.set(0, glm::inverse(viewProjectionMatrix));
	inverseTransform.submit();

	// bind all textures
	gbuffer.bindAlbedoTexture(lightingAlbedoSampler, 0);
	gbuffer.bindNormalTexture(lightingNormalSampler, 1);
	gbuffer.bindPbrTexture(lightingPbrSampler, 2);
	gbuffer.bindDepthTexture(lightingDepthSampler, 3);

	// run the shader
	lightingShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShader(lightingShader);
}
