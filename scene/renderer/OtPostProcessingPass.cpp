//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "glm/glm.hpp"

#include "OtPostProcessingPass.h"


//
//	OtPostProcessingPass::render
//

OtTexture* OtPostProcessingPass::render(OtSceneRendererContext& ctx) {
	// setup buffers
	postProcessBuffer1.update(ctx.camera.width, ctx.camera.height, OtTexture::Format::rgba16, OtTexture::Usage::rwDefault);
	postProcessBuffer2.update(ctx.camera.width, ctx.camera.height, OtTexture::Format::rgba16, OtTexture::Usage::rwDefault);
	OtTexture* input = &framebuffer.getColorTexture();
	OtTexture* output = &postProcessBuffer1;

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
			uv = glm::vec2(clipspace.x, -clipspace.y) * 0.5f + 0.5f;
		}
	}

	// local function to swap framebuffers
	auto swap = [&]() {
		if (input == &framebuffer.getColorTexture()) {
			input = &postProcessBuffer2;
		}

		std::swap(input, output);
	};

	// apply FXAA (Fast Approximate Anti-Aliasing) filter (if required)
	if (settings.fxaa) {
		fxaa.render(*input, *output);
		swap();
	}

	// apply fog (if required)
	if (settings.fog) {
		fog.setDepthTexture(framebuffer.getDepthTexture());
		fog.setInverseProjection(glm::inverse(ctx.camera.projectionMatrix));
		fog.setFogColor(settings.fogColor);
		fog.setFogDensity(settings.fogDensity);
		fog.setBackgroundFogRatio(settings.backgroundFogRatio);
		fog.render(*input, *output);
		swap();
	}

	// render and apply bloom (if required)
	if (settings.bloom) {
		// update bloom buffers
		for (auto i = 0; i < bloomDepth; i++) {
			bloomBuffer[i].update(
				ctx.camera.width >> (i + 1),
				ctx.camera.height >> (i + 1),
				OtTexture::Format::rgba16,
				OtTexture::Usage::rwDefault);
		}

		// down-sample
		for (auto i = 0; i < bloomDepth; i++) {
			bloomDownSample.render((i == 0) ? *input : bloomBuffer[i - 1], bloomBuffer[i]);
		}

		// up-sample
		bloomUpSample.setIntensity(settings.bloomIntensity);

		for (auto i = bloomDepth - 1; i > 0; i--) {
			bloomUpSample.render(bloomBuffer[i], bloomBuffer[i - 1]);
		}

		// setup pass to apply bloom
		OtBlitPass::blit(*input, *output);
		compositingAdd.render(bloomBuffer[0], *output);

		swap();
	}

	// render godrays (if required)
	if (settings.godrays) {
		// update occlusion buffer
		int width = ctx.camera.width / 2;
		int height = ctx.camera.height / 2;

		occlusionBuffer.update(
			width,
			height,
			OtTexture::Format::r8,
			OtTexture::Usage::rwDefault);

		// render light to occlusion buffer
		renderLight.setColor(ctx.directionalLightColor);
		renderLight.setCenter(uv);
		renderLight.setSize(glm::vec2(0.05f, 0.05f * width / height));
		renderLight.render(occlusionBuffer);

		// render entities to occlusion buffer
		auto camera = ctx.camera;
		ctx.camera.width = width;
		ctx.camera.height = height;
		occlusionPass.render(ctx);
		ctx.camera = camera;

		// render god rays
		godRays.setLightColor(glm::vec4(ctx.directionalLightColor, 0.6f));
		godRays.setLightUv(uv);
		godRays.setOcclusion(occlusionBuffer);
		godRays.render(*input, *output);

		swap();
	}

	// combine all post-processing effects
	postprocess.setExposure(settings.exposure);
	postprocess.setContrast(settings.contrast);
	postprocess.setTonemap(settings.tonemap);
	postprocess.render(*input, *output);
	return output;
}
