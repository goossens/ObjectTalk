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

	// get the directional light information
	glm::vec3 direction = glm::vec3(0.0);
	glm::vec3 color = glm::vec3(0.0);

	for (auto&& [entity, light, transform] : scene->view<OtDirectionalLightComponent, OtTransformComponent>().each()) {
		direction = transform.getTransform()[3];
		color = light.color;
	}

	// build the uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(camera->getPosition(), 0.0f);
	uniforms[1] = glm::vec4(direction, 0.0f);
	uniforms[2] = glm::vec4(color, 0.0f);

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
