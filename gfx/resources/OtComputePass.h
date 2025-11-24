//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <memory>
#include <vector>

#include "SDL3/SDL_gpu.h"

#include "OtComputePipeline.h"
#include "OtCubeMap.h"
#include "OtSampler.h"
#include "OtTexture.h"


//
//	OtComputePass
//

class OtComputePass {
public:
	// constructor
	OtComputePass();

	// add input samplers
	void addInputSampler(OtSampler& sampler, OtTexture& texture);
	void addInputSampler(OtSampler& sampler, OtCubeMap& cubemap);

	// add output textures
	void addOutputTexture(OtTexture& texture);
	void addOutputCubeMap(OtCubeMap& cubemap);
	void addOutputCubeMap(OtCubeMap& cubemap, size_t layer);

	// add uniforms
	void addUniforms(const void* data, size_t size);

	// execute a compute shader pass
	void execute(OtComputePipeline& pipeline, size_t groupCountX, size_t groupCountY, size_t groupCountZ);

private:
	// pass specific data
	std::vector<SDL_GPUTextureSamplerBinding> samplers;
	std::vector<SDL_GPUStorageTextureReadWriteBinding> outputTextures;
	std::vector<std::vector<std::byte>> uniforms;
};
