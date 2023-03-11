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
//	OtSceneRenderer::renderPostProcessingPass
//

void OtSceneRenderer::renderPostProcessingPass(OtScene* scene) {
	// get post-processing information
	float bloomIntensity = 0.0f;
	float exposure = 1.0f;

	for (auto [entity, component] : scene->view<OtPostProcessingComponent>().each()) {
		bloomIntensity = component.bloomIntensity;
		exposure = component.exposure;
	}

	// render bloom (if required)
	if (bloomIntensity > 0.0) {
		renderBloom(bloomIntensity);
	}

	// ensure the post-processing buffer is up to date
	postProcessBuffer.update(width, height);

	// combine all post-processing effects
	OtPass pass;
	pass.reserveRenderingSlot();
	pass.setFrameBuffer(postProcessBuffer);
	pass.submitQuad(width, height);

	// set uniform
	postProcessUniforms.set(0, glm::vec4(bloomIntensity > 0.0f, exposure, 0.0f, 0.0f));
	postProcessUniforms.submit();

	// set source textures
	compositeBuffer.bindColorTexture(postProcessSampler, 0);
	bloomBuffer[0].bindColorTexture(bloomSampler, 1);

	// run the shader
	postProcessShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShader(postProcessShader);
}


//
//	OtSceneRenderer::renderBloom
//

void OtSceneRenderer::renderBloom(float bloomIntensity) {
	// initialize bloom buffers (if required)
	if (!bloomBuffer[0].isValid()) {
		for (auto i = 0; i < bloomDepth; i++) {
			bloomBuffer[i].initialize(OtFrameBuffer::rgbaFloat16Texture, OtFrameBuffer::noTexture);
		}
	}

	// update bloom buffers
	for (auto i = 0; i < bloomDepth; i++) {
		bloomBuffer[i].update(width >> (i + 1), height >> (i + 1));
	}

	// downsample
	for (auto i = 0; i < bloomDepth; i++) {
		// setup pass
		int sw = width >> i;
		int sh = height >> i;
		int dw = sw >> 1;
		int dh = sh >> 1;

		OtPass pass;
		pass.reserveRenderingSlot();
		pass.setFrameBuffer(bloomBuffer[i]);
		pass.submitQuad(dw, dh);

		// set uniform
		bloomUniforms.set(0, glm::vec4(1.0 / sw, 1.0 / sh, 0.0f, 0.0f));
		bloomUniforms.submit();

		// set source texture
		if (i == 0) {
			compositeBuffer.bindColorTexture(bloomSampler, 0);

		} else {
			bloomBuffer[i - 1].bindColorTexture(bloomSampler, 0);
		}

		// run the shader
		bloomDownSampleShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
		pass.runShader(bloomDownSampleShader);
	}

	// upsample
	for (auto i = bloomDepth - 1; i > 0; i--) {
		// setup pass
		int dw = width >> i;
		int dh = height >> i;
		int sw = dw << 1;
		int sh = dh << 1;

		OtPass pass;
		pass.reserveRenderingSlot();
		pass.setFrameBuffer(bloomBuffer[i - 1]);
		pass.submitQuad(dw, dh);

		// set uniform
		bloomUniforms.set(0, glm::vec4(1.0 / sw, 1.0 / sh, bloomIntensity, 0.0f));
		bloomUniforms.submit();

		// set source texture
		bloomBuffer[i].bindColorTexture(bloomSampler, 0);

		// run the shader
		bloomUpSampleShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
		pass.runShader(bloomUpSampleShader);
	}
}
