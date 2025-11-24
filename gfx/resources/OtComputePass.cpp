//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "OtLog.h"

#include "OtComputePass.h"
#include "OtGpu.h"


//
//	OtComputePass::OtComputePass
//

OtComputePass::OtComputePass() {
	OtGpu::instance().computePasses++;
}


//
//	OtComputePass::addInputSampler
//

void OtComputePass::addInputSampler(OtSampler& sampler, OtTexture& texture) {
	if ((texture.getUsage() & OtTexture::sampler) == 0) {
		OtLogFatal("Can't add texture without [sampler] usage to compute pass");
	}

	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = texture.getTexture();
	binding.sampler = sampler.getSampler();
	samplers.emplace_back(binding);
}


//
//	OtComputePass::addInputSampler
//

void OtComputePass::addInputSampler(OtSampler& sampler, OtCubeMap& cubemap) {
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = cubemap.getTexture();
	binding.sampler = sampler.getSampler();
	samplers.emplace_back(binding);
}


//
//	OtComputePass::addOutputTexture
//

void OtComputePass::addOutputTexture(OtTexture& texture) {
	if ((texture.getUsage() & (OtTexture::Usage::computeStorageWrite | OtTexture::Usage::computeStorageReadWrite)) == 0) {
		OtLogFatal("Can't add output texture to compute pass without [write] usage");
	}

	SDL_GPUStorageTextureReadWriteBinding binding{};
	binding.texture = texture.getTexture();
	outputTextures.emplace_back(binding);
}


//
//	OtComputePass::addOutputCubeMap
//

void OtComputePass::addOutputCubeMap(OtCubeMap& cubemap) {
	SDL_GPUStorageTextureReadWriteBinding binding{};
	binding.texture = cubemap.getTexture();

	for (size_t i = 0; i < 6; i++) {
		binding.layer = static_cast<Uint32>(i);
		outputTextures.emplace_back(binding);
	}
}


//
//	OtComputePass::addOutputCubeMap
//

void OtComputePass::addOutputCubeMap(OtCubeMap& cubemap, size_t layer) {
	SDL_GPUStorageTextureReadWriteBinding binding{};
	binding.texture = cubemap.getTexture();
	binding.layer = static_cast<Uint32>(layer);
	outputTextures.emplace_back(binding);
}


//
//	OtComputePass::addUniforms
//

void OtComputePass::addUniforms(const void* data, size_t size) {
	auto& uniform = uniforms.emplace_back();
	uniform.resize(size);
	std::memcpy(uniform.data(), data, size);
}


//
//	OtComputePass::execute
//

void OtComputePass::execute(OtComputePipeline& pipeline, size_t groupCountX, size_t groupCountY, size_t groupCountZ) {
	SDL_GPUComputePass* pass = SDL_BeginGPUComputePass(
		OtGpu::instance().pipelineCommandBuffer,
		outputTextures.data(),
		static_cast<Uint32>(outputTextures.size()),
		nullptr,
		0);

	if (samplers.size()) {
		SDL_BindGPUComputeSamplers(pass, 0, samplers.data(), static_cast<Uint32>(samplers.size()));
	}

	SDL_BindGPUComputePipeline(pass, pipeline.getPipeline());

	for (size_t i = 0; i < uniforms.size(); i++) {
		SDL_PushGPUComputeUniformData(
			OtGpu::instance().pipelineCommandBuffer,
			static_cast<Uint32>(i),
			uniforms[i].data(),
			static_cast<Uint32>(uniforms[i].size()));
	}

	// run the pipeline
	SDL_DispatchGPUCompute(
		pass,
		static_cast<Uint32>(groupCountX),
		static_cast<Uint32>(groupCountY),
		static_cast<Uint32>(groupCountZ));

	// cleanup
	SDL_EndGPUComputePass(pass);
	samplers.clear();
	outputTextures.clear();
	uniforms.clear();
}
