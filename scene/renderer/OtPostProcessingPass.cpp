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

void OtSceneRenderer::renderPostProcessingPass(OtSceneRendererContext& ctx, bool renderPostProcessingEffects) {
	// get post-processing information
	float bloomIntensity = 0.0f;
	float exposure = 1.0f;

	if (renderPostProcessingEffects) {
		for (auto&& [entity, component] : ctx.scene->view<OtPostProcessingComponent>().each()) {
			bloomIntensity = component.bloomIntensity;
			exposure = component.exposure;
		}

		// render bloom (if required)
		if (bloomIntensity > 0.0f) {
			renderBloom(ctx, bloomIntensity);
		}
	}

	// ensure the post-processing buffer is up to date
	ctx.postProcessBuffer.update(ctx.width, ctx.height);

	// combine all post-processing effects
	OtPass pass;
	pass.setFrameBuffer(ctx.postProcessBuffer);
	pass.submitQuad(ctx.width, ctx.height);

	// set uniform
	postProcessUniforms.setValue(0, glm::vec4(bloomIntensity > 0.0f, exposure, 0.0f, 0.0f));
	postProcessUniforms.submit();

	// set source textures
	ctx.compositeBuffer.bindColorTexture(postProcessSampler, 0);
	bloomBuffer[0].bindColorTexture(bloomSampler, 1);

	// run the program
	postProcessProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShaderProgram(postProcessProgram);
}


//
//	OtSceneRenderer::renderBloom
//

void OtSceneRenderer::renderBloom(OtSceneRendererContext& ctx, float bloomIntensity) {
	// initialize bloom buffers (if required)
	if (!bloomBuffer[0].isValid()) {
		for (auto i = 0; i < bloomDepth; i++) {
			bloomBuffer[i].initialize(OtFrameBuffer::rgbaFloat16Texture, OtFrameBuffer::noTexture);
		}
	}

	// update bloom buffers
	for (auto i = 0; i < bloomDepth; i++) {
		bloomBuffer[i].update(ctx.width >> (i + 1), ctx.height >> (i + 1));
	}

	// downsample
	for (auto i = 0; i < bloomDepth; i++) {
		// setup pass
		int sw = ctx.width >> i;
		int sh = ctx.height >> i;
		int dw = sw >> 1;
		int dh = sh >> 1;

		OtPass pass;
		pass.setFrameBuffer(bloomBuffer[i]);
		pass.submitQuad(dw, dh);

		// set uniform
		bloomUniforms.setValue(0, glm::vec4(1.0 / sw, 1.0 / sh, 0.0f, 0.0f));
		bloomUniforms.submit();

		// set source texture
		if (i == 0) {
			ctx.compositeBuffer.bindColorTexture(bloomSampler, 0);

		} else {
			bloomBuffer[i - 1].bindColorTexture(bloomSampler, 0);
		}

		// run the program
		bloomDownSampleProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
		pass.runShaderProgram(bloomDownSampleProgram);
	}

	// upsample
	for (auto i = bloomDepth - 1; i > 0; i--) {
		// setup pass
		int dw = ctx.width >> i;
		int dh = ctx.height >> i;
		int sw = dw >> 1;
		int sh = dh >> 1;

		OtPass pass;
		pass.setFrameBuffer(bloomBuffer[i - 1]);
		pass.submitQuad(dw, dh);

		// set uniform
		bloomUniforms.setValue(0, glm::vec4(1.0 / sw, 1.0 / sh, bloomIntensity, 0.0f));
		bloomUniforms.submit();

		// set source texture
		bloomBuffer[i].bindColorTexture(bloomSampler, 0);

		// run the program
		bloomUpSampleProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
		pass.runShaderProgram(bloomUpSampleProgram);
	}
}
