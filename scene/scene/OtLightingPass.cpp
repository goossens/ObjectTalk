//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/matrix_transform.hpp"

#include "OtQuad.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderLightingPass
//

void OtSceneRenderer::renderLightingPass(OtScene2 scene) {
	// setup pass
	lightingPass.reserveRenderingSlot();
	lightingPass.setClear(false, false);
	lightingPass.setRectangle(0, 0, width, height);
	lightingPass.setFrameBuffer(composite);
	lightingPass.setTransform(glm::mat4(1.0), glm::ortho(0.0f, (float) width, (float) height, 0.0f, -1.0f, 1.0f));

	// submit geometry
	OtQuadSubmit(width, height);

	// submit uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(camera->getPosition(), 0.0f);
	uniforms[1] = glm::vec4(10.0f, 10.0f, 10.0f, 0.0f);
	uniforms[2] = glm::vec4(1.0f);
	lightingUniforms.submit();

	// bind all textures
	gbuffer.bindAlbedoTexture(lightingAlbedoSampler, 0);
	gbuffer.bindPositionTexture(lightingPositionSampler, 1);
	gbuffer.bindNormalTexture(lightingNormalSampler, 2);
	gbuffer.bindPbrTexture(lightingPbrSampler, 3);
	gbuffer.bindDepthTexture(lightingDepthSampler, 4);

	// load the shader (if required)
	if (!lightingShader.isValid()) {
		lightingShader.initialize("OtLightingVS", "OtLightingFS");
	}

	// run the shader
	lightingShader.setState(OtShader::blendAlpha);
	lightingPass.runShader(lightingShader);
}
