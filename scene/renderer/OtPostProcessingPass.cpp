//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderPostProcessingPass
//

void OtSceneRenderer::renderPostProcessingPass(OtSceneRendererContext& ctx) {
	// setup buffers
	postProcessBuffer1.update(ctx.camera.width, ctx.camera.height);
	postProcessBuffer2.update(ctx.camera.width, ctx.camera.height);
	OtFrameBuffer* input = &compositeBuffer;
	OtFrameBuffer* output = &postProcessBuffer1;

	// get post-processing information
	bool fxaa = false;
	bool fog = false;
	float fogDensity;
	glm::vec3 fogColor;
	float bloomIntensity = 0.0f;
	float exposure = 1.0f;

	for (auto&& [entity, component] : ctx.scene->view<OtPostProcessingComponent>().each()) {
		fxaa = component.fxaa;
		fog = component.fog;
		fogDensity = component.fogDensity;
		fogColor = component.fogColor;
		bloomIntensity = component.bloomIntensity;
		exposure = component.exposure;
	}

	// local function to swap framebuffers
	auto swap = [&]() {
		if (input == &compositeBuffer) {
			input = &postProcessBuffer2;
		}

		std::swap(input, output);
	};

	// apply FXAA (Fast Approximate Anti-Aliasing) filter (if required)
	if (fxaa) {
		renderFxaa(ctx, input, output);
		swap();
	}

	// apply fog (if required)
	if (fog) {
		renderFog(ctx, input, output, fogDensity, fogColor);
		swap();
	}

	// render and apply bloom (if required)
	if (bloomIntensity > 0.0f) {
		// render bloom by down and up-sampling
		renderBloom(ctx, input, output, bloomIntensity);
		swap();
	}

	// combine all post-processing effects
	OtPass pass;
	pass.setFrameBuffer(*output);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

	// set uniform
	postProcessUniforms.setValue(0, exposure, 0.0f, 0.0f, 0.0f);
	postProcessUniforms.submit();

	// set source textures
	input->bindColorTexture(postProcessSampler, 0);

	// run the program
	postProcessProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShaderProgram(postProcessProgram);

	// mark the right output buffer;
	ctx.output = output;
}


//
//	OtSceneRenderer::renderFxaa
//

void OtSceneRenderer::renderFxaa(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output) {
	// setup pass
	OtPass pass;
	pass.setFrameBuffer(*output);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

	// set source texture
	input->bindColorTexture(postProcessSampler, 0);

	// set uniform
	fxaaUniforms.setValue(0, 1.0f / float(ctx.camera.width), 1.0f / float(ctx.camera.height), 0.0f, 0.0f);
	fxaaUniforms.submit();

	// run the program
	fxaaProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShaderProgram(fxaaProgram);
}


//
//	OtSceneRenderer::renderFog
//

void OtSceneRenderer::renderFog(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, float fogDensity, glm::vec3& fogColor) {
	// setup pass
	OtPass pass;
	pass.setFrameBuffer(*output);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

	// set source texture
	input->bindColorTexture(postProcessSampler, 0);
	ctx.compositeBuffer.bindDepthTexture(depthSampler, 1);

	// set uniforms
	fogUniforms.setValue(0, fogDensity, 0.0f, 0.0f, 0.0f);
	fogUniforms.setValue(1, fogColor, 0.0f);
	fogUniforms.submit();

	invProjUniform.set(0, glm::inverse(ctx.camera.projectionMatrix));
	invProjUniform.submit();

	// run the program
	fxaaProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	pass.runShaderProgram(fogProgram);
}


//
//	OtSceneRenderer::renderBloom
//

void OtSceneRenderer::renderBloom(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, float bloomIntensity) {
	// initialize bloom buffers (if required)
	if (!bloomBuffer[0].isValid()) {
		for (auto i = 0; i < bloomDepth; i++) {
			bloomBuffer[i].initialize(OtTexture::rgbaFloat16Texture, OtTexture::noTexture);
		}
	}

	// update bloom buffers
	for (auto i = 0; i < bloomDepth; i++) {
		bloomBuffer[i].update(ctx.camera.width >> (i + 1), ctx.camera.height >> (i + 1));
	}

	// downsample
	for (auto i = 0; i < bloomDepth; i++) {
		// setup pass
		int sw = ctx.camera.width >> i;
		int sh = ctx.camera.height >> i;
		int dw = sw >> 1;
		int dh = sh >> 1;

		OtPass pass;
		pass.setFrameBuffer(bloomBuffer[i]);
		pass.submitQuad(dw, dh);

		// set uniform
		bloomUniforms.setValue(0, 1.0f / float(sw), 1.0f / float(sh), 0.0f, 0.0f);
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
		int dw = ctx.camera.width >> i;
		int dh = ctx.camera.height >> i;
		int sw = dw >> 1;
		int sh = dh >> 1;

		OtPass pass;
		pass.setFrameBuffer(bloomBuffer[i - 1]);
		pass.submitQuad(dw, dh);

		// set uniform
		bloomUniforms.setValue(0, 1.0f / float(sw), 1.0f / float(sh), bloomIntensity, 0.0f);
		bloomUniforms.submit();

		// set source texture
		bloomBuffer[i].bindColorTexture(bloomSampler, 0);

		// run the program
		bloomUpSampleProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
		pass.runShaderProgram(bloomUpSampleProgram);
	}

	// setup pass to apply bloom
	OtPass applyBloom;
	applyBloom.setFrameBuffer(*output);
	applyBloom.submitQuad(ctx.camera.width, ctx.camera.height);

	// set source textures
	input->bindColorTexture(postProcessSampler, 0);
	bloomBuffer[0].bindColorTexture(bloomSampler, 1);

	// run the program
	bloomApplyProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateDepthTestAlways);
	applyBloom.runShaderProgram(bloomApplyProgram);
}
