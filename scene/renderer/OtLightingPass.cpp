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

void OtSceneRenderer::renderLightingPass(OtScene2 scene) {
	// setup pass
	OtPass pass;
	pass.reserveRenderingSlot();
	pass.setFrameBuffer(compositeBuffer);
	pass.submitQuad(width, height);

	// get the post-processing information
	float exposure = 1.0f;
	bool fogEnabled = false;
	glm::vec3 fogColor{0.0f};
	float fogNear = 0.1;
	float fogFar = 1000.0;

	for (auto [entity, component] : scene->view<OtPostProcessingComponent>().each()) {
		exposure = component.exposure;
		fogEnabled = component.fogEnabled;
		fogColor = component.fogColor;
		fogNear = component.fogNear;
		fogFar = component.fogFar;
	}

	// build the uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(camera->getPosition(), exposure);
	uniforms[1] = glm::vec4(10.0f, 10.0f, 10.0f, 0.0f);
	uniforms[2] = glm::vec4(1.0f);
	uniforms[3] = glm::vec4(fogEnabled, fogNear, fogFar, 0.0f);
	uniforms[4] = glm::vec4(fogColor, 0.0f);

	// submit uniforms
	lightingUniforms.submit();

	// bind all textures
	gbuffer.bindAlbedoTexture(lightingAlbedoSampler, 0);
	gbuffer.bindPositionTexture(lightingPositionSampler, 1);
	gbuffer.bindNormalTexture(lightingNormalSampler, 2);
	gbuffer.bindPbrTexture(lightingPbrSampler, 3);

	// run the shader
	lightingShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShader(lightingShader);
}
