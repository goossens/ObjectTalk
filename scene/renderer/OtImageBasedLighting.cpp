//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "OtComputePass.h"
#include "OtRenderPass.h"

#include "OtImageBasedLighting.h"

#include "OtFullScreenVert.h"
#include "OtIblIrradianceMapFrag.h"
#include "OtIblEnvironmentMapFrag.h"


//
//	OtImageBasedLighting::update
//

void OtImageBasedLighting::update(OtIblComponent& component) {
	// see if the cubemap has changed
	OtCubeMap& cubemap = component.cubemap->getCubeMap();

	if (cubemap.getVersion() != iblSkyMapVersion) {
		// initialize resources (if required)
		if (!resourcesInitialized) {
			initializeResources();
			resourcesInitialized = true;
		}

		static constexpr int threadCount = 16;
		static constexpr int brdfLutSize = 128;
		static constexpr int irradianceSize = 64;
		static constexpr int environmentSize = 256;
		static constexpr int environmentMipLevels = 8;

		// generate the Smith BRDF LUT
		auto usage = OtTexture::Usage(OtTexture::Usage::sampler | OtTexture::Usage::computeStorageWrite);
		iblBrdfLut.update(brdfLutSize, brdfLutSize, OtTexture::Format::rg16, usage);

		OtComputePass brdfPass;
		brdfPass.addOutputTexture(iblBrdfLut);
		brdfPass.execute(brdfLutPipeline, brdfLutSize / threadCount, brdfLutSize / threadCount, 1);

		// render irradiance map
		OtRenderPass irradiancePass;
		iblIrradianceMap.create(OtCubeMap::Format::rgba16, irradianceSize, false);

		irradiancePass.start(iblIrradianceMap);
		irradiancePass.bindPipeline(irradiancePipeline);
		irradiancePass.bindFragmentSampler(0, cubemapSampler, cubemap);
		irradiancePass.render(3);
		irradiancePass.end();

		// render environment map
		iblEnvironmentMap.create(OtCubeMap::Format::rgba16, environmentSize, true);
		maxEnvLevel = environmentMipLevels;

		for (auto mipLevel = 0; mipLevel < environmentMipLevels; mipLevel++) {
			OtRenderPass environmentPass;

			environmentPass.start(iblEnvironmentMap, mipLevel);
			environmentPass.bindPipeline(environmentPipeline);

			// set fragment uniforms
			struct Uniforms {
				float roughness;
				uint32_t environmentSize;
			} uniforms{
				static_cast<float>(mipLevel) / static_cast<float>(environmentMipLevels),
				static_cast<uint32_t>(environmentSize)
			};

			environmentPass.setFragmentUniforms(0, &uniforms, sizeof(uniforms));
			environmentPass.bindFragmentSampler(0, cubemapSampler, cubemap);
			environmentPass.render(3);
			environmentPass.end();
		}

		// update baseline
		iblSkyMapVersion = cubemap.getVersion();
		iblSkyMap = &cubemap;
	}
}


//
//	OtImageBasedLighting::initializeResources
//

void OtImageBasedLighting::initializeResources() {
	irradiancePipeline.setShaders(OtFullScreenVert, sizeof(OtFullScreenVert), OtIblIrradianceMapFrag, sizeof(OtIblIrradianceMapFrag));
	irradiancePipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::cubemap);

	environmentPipeline.setShaders(OtFullScreenVert, sizeof(OtFullScreenVert), OtIblEnvironmentMapFrag, sizeof(OtIblEnvironmentMapFrag));
	environmentPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::cubemap);
}
