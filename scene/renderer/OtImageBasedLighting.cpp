//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPass.h"

#include "OtImageBasedLighting.h"


//
//	OtImageBasedLighting::update
//

void OtImageBasedLighting::update(OtIblComponent& component) {
	// see if the cubemap has changed
	OtCubeMap& cubemap = component.cubemap->getCubeMap();

	if (cubemap.getVersion() != iblSkyMapVersion) {
		static constexpr int threadCount = 8;
		static constexpr int brdfLutSize = 128;
		static constexpr int irradianceSize = 64;
		static constexpr int environmentSize = 256;
		static constexpr int environmentMipLevels = 8;

		// generate the Smith BRDF LUT
		uint64_t flags =
			OtTexture::pointSampling |
			OtTexture::clampSampling |
			OtTexture::computeWrite;

		iblBrdfLut.create(brdfLutSize, brdfLutSize, OtTexture::rgFloat16Texture, flags);

		// our pass
		OtPass pass;
		pass.setImage(0, iblBrdfLut, 0, OtPass::writeAccess);
		pass.runComputeProgram(brdfLutProgram, brdfLutSize / threadCount, brdfLutSize / threadCount, 1);

		// render irradiance map
		iblIrradianceMap.create(irradianceSize, false, 1, OtTexture::rgbaFloat16Texture, OtTexture::computeWrite);
		cubemapSampler.submit(0, cubemap);
		pass.setImage(1, iblIrradianceMap, 0, OtPass::writeAccess);
		pass.runComputeProgram(irradianceProgram, irradianceSize / threadCount, irradianceSize / threadCount, 1);

		// render environment map
		iblEnvironmentMap.create(environmentSize, true, 1, OtTexture::rgbaFloat16Texture, OtTexture::computeWrite);
		maxEnvLevel = environmentMipLevels;

		for (auto mipLevel = 0; mipLevel <= environmentMipLevels; mipLevel++) {
			int mipSize = environmentSize / (1 << mipLevel);
			float roughness = float(mipLevel) / float(environmentMipLevels);

			iblEnvironmentUniform.setValue(0, roughness, float(mipLevel), float(environmentSize), 0.0f);
			iblEnvironmentUniform.submit();
			cubemapSampler.submit(0, cubemap);
			pass.setImage(1, iblEnvironmentMap, mipLevel, OtPass::writeAccess);
			pass.runComputeProgram(envmapProgram, mipSize / threadCount, mipSize / threadCount, 1);
		}

		iblSkyMapVersion = cubemap.getVersion();
		iblSkyMap = &cubemap;
	}
}