//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtRenderLight.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"

#include "OtOcclusionPass.h"
#include "OtSceneRenderPass.h"


//
//	OtPostProcessingPass
//

class OtPostProcessingPass : public OtSceneRenderPass {
public:
	// constructor
	OtPostProcessingPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	int render(OtSceneRendererContext& ctx) {
		// setup buffers
		postProcessBuffer1.update(ctx.camera.width, ctx.camera.height);
		postProcessBuffer2.update(ctx.camera.width, ctx.camera.height);
		OtFrameBuffer* input = &framebuffer;
		OtFrameBuffer* output = &postProcessBuffer1;

		// get post processing settings
		auto settings = ctx.scene->getPostProcessing();

		// do some special processing for godrays
		glm::vec2 uv;

		if (settings.godrays) {
			// determine light position in clipspace
			glm::vec4 viewspace = ctx.camera.viewMatrix * glm::vec4(ctx.directionalLightDirection, 0.0f);
			glm::vec4 clipspace = ctx.camera.projectionMatrix * viewspace;
			clipspace /= clipspace.w;

			// see if it has any effect
			if (viewspace.z > 0.0f || clipspace.x < -1.5f || clipspace.x > 1.5f || clipspace.y < -1.5f || clipspace.y > 1.5f) {
				// nope, just turn it off for this frame
				settings.godrays = false;

			} else {
				// convert to UV
				uv = glm::vec2(clipspace.x, clipspace.y) * 0.5f + 0.5f;

				if (!OtGpuHasOriginBottomLeft()) {
					uv.y = 1.0f - uv.y;
				}
			}
		}

		// local function to swap framebuffers
		auto swap = [&]() {
			if (input == &framebuffer) {
				input = &postProcessBuffer2;
			}

			std::swap(input, output);
		};

		// apply FXAA (Fast Approximate Anti-Aliasing) filter (if required)
		if (settings.fxaa) {
			renderFxaa(ctx, input, output);
			swap();
		}

		// apply fog (if required)
		if (settings.fog) {
			renderFog(ctx, input, output, settings.fogDensity, settings.fogColor);
			swap();
		}

		// render and apply bloom (if required)
		if (settings.bloomIntensity > 0.0f) {
			// render bloom by down and up-sampling
			renderBloom(ctx, input, output, settings.bloomIntensity);
			swap();
		}

		// render godrays (if required)
		if (settings.godrays) {
			renderGodrays(ctx, input, output, uv);
			swap();
		}

		// combine all post-processing effects
		OtPass pass;
		pass.setFrameBuffer(*output);
		pass.submitQuad(ctx.camera.width, ctx.camera.height);

		// set uniform
		postProcessUniforms.setValue(0, settings.exposure, settings.contrast, float(settings.tonemap), 0.0f);
		postProcessUniforms.submit();

		// set source textures
		input->bindColorTexture(postProcessSampler, 0);

		// run the program
		postProcessProgram.setState(OtStateWriteRgb | OtStateWriteA);
		pass.runShaderProgram(postProcessProgram);

		// mark the right output buffer;
		return output->getColorTextureIndex();
	}

	// apply FXAA (Fast Approximate Anti-Aliasing) filter
	void renderFxaa(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output) {
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
		fxaaProgram.setState(OtStateWriteRgb | OtStateWriteA);
		pass.runShaderProgram(fxaaProgram);
	}

	// apply fog
	void renderFog(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, float fogDensity, glm::vec3& fogColor) {
		// setup pass
		OtPass pass;
		pass.setFrameBuffer(*output);
		pass.submitQuad(ctx.camera.width, ctx.camera.height);

		// set source texture
		input->bindColorTexture(postProcessSampler, 0);
		framebuffer.bindDepthTexture(depthSampler, 1);

		// set uniforms
		fogUniforms.setValue(0, fogDensity, 0.0f, 0.0f, 0.0f);
		fogUniforms.setValue(1, fogColor, 0.0f);
		fogUniforms.submit();

		invProjUniform.set(0, glm::inverse(ctx.camera.projectionMatrix));
		invProjUniform.submit();

		// run the program
		fxaaProgram.setState(OtStateWriteRgb | OtStateWriteA);
		pass.runShaderProgram(fogProgram);
	}

	// apply bloom
	void renderBloom(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, float bloomIntensity) {
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

		// down sample
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
				framebuffer.bindColorTexture(bloomSampler, 0);

			} else {
				bloomBuffer[i - 1].bindColorTexture(bloomSampler, 0);
			}

			// run the program
			bloomDownSampleProgram.setState(OtStateWriteRgb | OtStateWriteA);
			pass.runShaderProgram(bloomDownSampleProgram);
		}

		// up sample
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
			bloomUpSampleProgram.setState(OtStateWriteRgb | OtStateWriteA);
			pass.runShaderProgram(bloomUpSampleProgram);
		}

		// setup pass to apply bloom
		OtPass pass;
		pass.setFrameBuffer(*output);
		pass.submitQuad(ctx.camera.width, ctx.camera.height);

		// set source textures
		input->bindColorTexture(postProcessSampler, 0);
		bloomBuffer[0].bindColorTexture(bloomSampler, 1);

		// run the program
		bloomApplyProgram.setState(OtStateWriteRgb | OtStateWriteA);
		pass.runShaderProgram(bloomApplyProgram);
	}

	void renderGodrays(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, glm::vec2& uv) {
		// update occlusion buffer
		int width = ctx.camera.width / 2;
		int height = ctx.camera.height / 2;
		occlusionBuffer.update(width, height);

		// render light to occlusion buffer
		renderLight.setCenter(uv);
		renderLight.setSize(glm::vec2(0.05f, 0.05f * width / height));
		renderLight.setColor(ctx.directionalLightColor);
		renderLight.render(occlusionBuffer);

		// create an occlusion camera
		OtCamera camera{ctx.camera};
		camera.width = width;
		camera.height = height;

		// render all objects to the occlusion buffer
		OtSceneRendererContext octx{ctx};
		octx.camera = camera;
		occlusionPass.render(octx);

		// setup godray pass
		OtPass pass;
		pass.setFrameBuffer(*output);
		pass.submitQuad(ctx.camera.width, ctx.camera.height);

		// set the uniforms
		godrayUniforms.setValue(0, uv, 0.0f, 0.0f);
		godrayUniforms.setValue(1, ctx.directionalLightColor, 0.6f);
		godrayUniforms.setValue(2, 0.92f, 0.5f, 0.9f, 0.5f);
		godrayUniforms.submit();

		// bind the textures
		input->bindColorTexture(postProcessSampler, 0);
		occlusionBuffer.bindColorTexture(occlusionSampler, 1);

		// run the program
		godrayProgram.setState(OtStateWriteRgb | OtStateWriteA);
		pass.runShaderProgram(godrayProgram);
	}

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// properties
	OtFrameBuffer& framebuffer;
	OtFrameBuffer postProcessBuffer1{OtTexture::rgbaFloat16Texture, OtTexture::noTexture};
	OtFrameBuffer postProcessBuffer2{OtTexture::rgbaFloat16Texture, OtTexture::noTexture};
	OtFrameBuffer occlusionBuffer{OtTexture::r8Texture, OtTexture::noTexture};

	OtRenderLight renderLight;
	OtOcclusionPass occlusionPass{occlusionBuffer};

	static constexpr int bloomDepth = 5;
	OtFrameBuffer bloomBuffer[bloomDepth];

	OtUniformVec4 fxaaUniforms{"u_fxaa", 1};
	OtUniformVec4 fogUniforms{"u_fog", 2};
	OtUniformVec4 bloomUniforms{"u_bloom", 1};
	OtUniformVec4 godrayUniforms{"u_godrays", 3};
	OtUniformVec4 postProcessUniforms{"u_postProcess", 1};

	OtUniformMat4 invProjUniform{"u_invProjUniform", 1};

	OtSampler postProcessSampler{"s_postProcessTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler depthSampler{"s_depthTexture"};
	OtSampler bloomSampler{"s_bloomTexture"};
	OtSampler occlusionSampler{"s_occlusionTexture"};

	OtShaderProgram fxaaProgram{"OtFilterVS", "OtFxaaFS"};
	OtShaderProgram fogProgram{"OtFilterVS", "OtFogFS"};
	OtShaderProgram bloomDownSampleProgram{"OtFilterVS", "OtBloomDownSampleFS"};
	OtShaderProgram bloomUpSampleProgram{"OtFilterVS", "OtBloomUpSampleFS"};
	OtShaderProgram bloomApplyProgram{"OtFilterVS", "OtBloomApplyFS"};
	OtShaderProgram godrayProgram{"OtGodraysVS", "OtGodraysFS"};
	OtShaderProgram postProcessProgram{"OtFilterVS", "OtPostProcessFS"};
};
