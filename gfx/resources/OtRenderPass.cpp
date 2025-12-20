//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtAssert.h"
#include "OtLog.h"

#include "OtGpu.h"
#include "OtRenderPass.h"


//
//	OtRenderPass::OtRenderPass
//

OtRenderPass::OtRenderPass() {
	OtGpu::instance().renderPasses++;
}


//
//	OtRenderPass::~OtRenderPass
//

OtRenderPass::~OtRenderPass() {
	OtAssert(!open);
}


//
//	OtRenderPass::setClearColor
//

void OtRenderPass::setClearColor(bool flag, const glm::vec4& value) {
	OtAssert(!open);
	clearColorTexture = flag;
	clearColorValue = value;
}


//
//	OtRenderPass::setClearDepth
//

void OtRenderPass::setClearDepth(bool flag, float value) {
	OtAssert(!open);
	clearDepthTexture = flag;
	clearDepthValue = value;
}


//
//	OtRenderPass::setClearStencil
//

void OtRenderPass::setClearStencil(bool flag, uint8_t value) {
	OtAssert(!open);
	clearStencilTexture = flag;
	clearStencilValue = value;
}


//
//	OtRenderPass::start
//

void OtRenderPass::start(OtTexture& texture) {
	// sanity checks
	OtAssert(!open);

	if (!texture.isValid()) {
		OtLogFatal("Can't use invalid texture in render pass");
	}

	// start rendering pass
	SDL_GPUColorTargetInfo info{};
	info.texture = texture.getTexture();

	if (clearColorTexture) {
		info.load_op = SDL_GPU_LOADOP_CLEAR;
		info.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
	}

	pass = SDL_BeginGPURenderPass(OtGpu::instance().pipelineCommandBuffer, &info, 1, nullptr);

	if (!pass) {
		OtLogFatal("Error in SDL_BeginGPURenderPass: {}", SDL_GetError());
	}

	open = true;
}

void OtRenderPass::start(OtFrameBuffer& framebuffer) {
	// sanity checks
	OtAssert(!open);

	if (!framebuffer.isValid()) {
		OtLogFatal("Can't use invalid framebuffer in render pass");
	}

	// get render description
	auto info = framebuffer.getRenderTargetInfo(
		clearColorTexture,
		clearDepthTexture,
		clearStencilTexture,
		clearColorValue,
		clearDepthValue,
		clearStencilValue);

	// start rendering pass
	pass = SDL_BeginGPURenderPass(
		OtGpu::instance().pipelineCommandBuffer,
		info->colorTargetInfo,
		info->numColorTargets,
		info->depthStencilTargetInfo);

	if (!pass) {
		OtLogFatal("Error in SDL_BeginGPURenderPass: {}", SDL_GetError());
	}

	open = true;
}

void OtRenderPass::start(OtGbuffer& gbuffer) {
	// sanity checks
	OtAssert(!open);

	if (!gbuffer.isValid()) {
		OtLogFatal("Can't use invalid gbuffer in render pass");
	}

	// start rendering pass
	auto info = gbuffer.getRenderTargetInfo(
		clearColorTexture,
		clearDepthTexture,
		clearStencilTexture,
		clearColorValue,
		clearDepthValue,
		clearStencilValue);

	pass = SDL_BeginGPURenderPass(
		OtGpu::instance().pipelineCommandBuffer,
		info->colorTargetInfo,
		info->numColorTargets,
		info->depthStencilTargetInfo);

	if (!pass) {
		OtLogFatal("Error in SDL_BeginGPURenderPass: {}", SDL_GetError());
	}

	open = true;
}


//
//	OtRenderPass::start
//

void OtRenderPass::start(OtCubeMap& cubemap, size_t mipLevel) {
	// sanity checks
	OtAssert(!open);

	if (!cubemap.isValid()) {
		OtLogFatal("Can't use invalid cubemap in render pass");
	}

	// start rendering pass
	SDL_GPUColorTargetInfo colorTargetInfo[6];

	colorTargetInfo[0] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[0].texture = cubemap.getTexture();
	colorTargetInfo[0].mip_level = static_cast<Uint32>(mipLevel);
	colorTargetInfo[0].layer_or_depth_plane = 0;

	colorTargetInfo[1] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[1].texture = cubemap.getTexture();
	colorTargetInfo[1].mip_level = static_cast<Uint32>(mipLevel);
	colorTargetInfo[1].layer_or_depth_plane = 1;

	colorTargetInfo[2] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[2].texture = cubemap.getTexture();
	colorTargetInfo[2].mip_level = static_cast<Uint32>(mipLevel);
	colorTargetInfo[2].layer_or_depth_plane = 2;

	colorTargetInfo[3] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[3].texture = cubemap.getTexture();
	colorTargetInfo[3].mip_level = static_cast<Uint32>(mipLevel);
	colorTargetInfo[3].layer_or_depth_plane = 3;

	colorTargetInfo[4] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[4].texture = cubemap.getTexture();
	colorTargetInfo[4].mip_level = static_cast<Uint32>(mipLevel);
	colorTargetInfo[4].layer_or_depth_plane = 4;

	colorTargetInfo[5] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[5].texture = cubemap.getTexture();
	colorTargetInfo[5].mip_level = static_cast<Uint32>(mipLevel);
	colorTargetInfo[5].layer_or_depth_plane = 5;

	pass = SDL_BeginGPURenderPass(OtGpu::instance().pipelineCommandBuffer, colorTargetInfo, 6, nullptr);

	if (!pass) {
		OtLogFatal("Error in SDL_BeginGPURenderPass: {}", SDL_GetError());
	}

	open = true;
}


//
//	OtRenderPass::bindPipeline
//

void OtRenderPass::bindPipeline(OtRenderPipeline& pipeline) {
	OtAssert(open);
	SDL_BindGPUGraphicsPipeline(pass, pipeline.getPipeline());
}


//
//	OtRenderPass::bindVertexSampler
//

void OtRenderPass::bindVertexSampler(size_t slot, OtSampler& sampler, SDL_GPUTexture* texture) {
	OtAssert(open);
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = texture;
	binding.sampler = sampler.getSampler();
	SDL_BindGPUVertexSamplers(pass, static_cast<Uint32>(slot), &binding, 1);
}

void OtRenderPass::bindVertexSampler(size_t slot, OtSampler& sampler, OtTexture& texture) {
	OtAssert(open);
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = texture.getTexture();
	binding.sampler = sampler.getSampler();
	SDL_BindGPUVertexSamplers(pass, static_cast<Uint32>(slot), &binding, 1);
}

void OtRenderPass::bindVertexSampler(size_t slot, OtSampler& sampler, OtCubeMap& cubemap) {
	OtAssert(open);
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = cubemap.getTexture();
	binding.sampler = sampler.getSampler();
	SDL_BindGPUVertexSamplers(pass, static_cast<Uint32>(slot), &binding, 1);
}


//
//	OtRenderPass::bindFragmentSampler
//

void OtRenderPass::bindFragmentSampler(size_t slot, OtSampler& sampler, SDL_GPUTexture* texture) {
	OtAssert(open);
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = texture;
	binding.sampler = sampler.getSampler();
	SDL_BindGPUFragmentSamplers(pass, static_cast<Uint32>(slot), &binding, 1);
}

void OtRenderPass::bindFragmentSampler(size_t slot, OtSampler& sampler, OtTexture& texture) {
	OtAssert(open);
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = texture.getTexture();
	binding.sampler = sampler.getSampler();
	SDL_BindGPUFragmentSamplers(pass, static_cast<Uint32>(slot), &binding, 1);
}

void OtRenderPass::bindFragmentSampler(size_t slot, OtSampler& sampler, OtCubeMap& cubemap) {
	OtAssert(open);
	SDL_GPUTextureSamplerBinding binding{};
	binding.texture = cubemap.getTexture();
	binding.sampler = sampler.getSampler();
	SDL_BindGPUFragmentSamplers(pass, static_cast<Uint32>(slot), &binding, 1);
}


//
//	OtRenderPass::bindVertexUniforms
//

void OtRenderPass::bindVertexUniforms(size_t slot, const void* data, size_t size) {
	OtAssert(open);

	SDL_PushGPUVertexUniformData(
		OtGpu::instance().pipelineCommandBuffer,
		static_cast<Uint32>(slot),
		data,
		static_cast<Uint32>(size));
}


//
//	OtRenderPass::bindFragmentUniforms
//

void OtRenderPass::bindFragmentUniforms(size_t slot, const void* data, size_t size) {
	OtAssert(open);

	SDL_PushGPUFragmentUniformData(
		OtGpu::instance().pipelineCommandBuffer,
		static_cast<Uint32>(slot),
		data,
		static_cast<Uint32>(size));
}


//
//	OtRenderPass::setStencilReference
//

void OtRenderPass::setStencilReference(uint8_t reference) {
	OtAssert(open);
	SDL_SetGPUStencilReference(pass, static_cast<Uint8>(reference));
}


//
//	OtRenderPass::setInstanceData
//

void OtRenderPass::setInstanceData(OtVertexBuffer& idb) {
	OtAssert(open);
	instanceBuffer = idb.getBuffer();
	instanceCount = idb.getCount();
}


//
//	OtRenderPass::setInstanceCount
//

void OtRenderPass::setInstanceCount(size_t instances) {
	instanceCount = instances;
}


//
//	OtRenderPass::setAnimationData
//

void OtRenderPass::setAnimationData(OtVertexBuffer& adb) {
	animationBuffer = adb.getBuffer();
}


//
//	OtRenderPass::render
//

void OtRenderPass::render(size_t vertices, size_t instances) {
	OtAssert(open);
	SDL_DrawGPUPrimitives(pass, static_cast<Uint32>(vertices), static_cast<Uint32>(instances), 0, 0);
}

void OtRenderPass::render(OtVertexBuffer& vertexBuffer) {
	OtAssert(open);
	SDL_GPUBufferBinding bufferBinding{vertexBuffer.getBuffer(), 0};
	SDL_BindGPUVertexBuffers(pass, 0, &bufferBinding, 1);
	SDL_DrawGPUPrimitives(pass, static_cast<Uint32>(vertexBuffer.getCount()), 1, 0, 0);
}

void OtRenderPass::render(OtVertexBuffer& vertexBuffer, OtIndexBuffer& indexBuffer, size_t offset, size_t count) {
	// bind the vertex buffer(s) to the pass
	OtAssert(open);
	SDL_GPUBufferBinding bufferBindings[2];
	bufferBindings[0] = SDL_GPUBufferBinding{vertexBuffer.getBuffer(), 0};

	if (animationBuffer) {
		bufferBindings[1] = SDL_GPUBufferBinding{animationBuffer, 0};

	} else if (instanceBuffer) {
		bufferBindings[1] = SDL_GPUBufferBinding{instanceBuffer, 0};
	}

	SDL_BindGPUVertexBuffers(pass, 0, bufferBindings, (animationBuffer || instanceBuffer) ? 2 : 1);

	// bind the index buffer to the pass
	SDL_GPUBufferBinding indexBufferBinding = {indexBuffer.getBuffer(), 0};
	SDL_BindGPUIndexBuffer(pass, &indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

	// render the triangles
	Uint32 numIndices = static_cast<Uint32>(count == 0 ? indexBuffer.getCount() : count);
	Uint32 firstIndex = static_cast<Uint32>(offset);
	Uint32 numInstances = static_cast<Uint32>(instanceCount);
	SDL_DrawGPUIndexedPrimitives(pass, numIndices, numInstances, firstIndex, 0, 0);
	animationBuffer = nullptr;
	instanceBuffer = nullptr;
	instanceCount = 1;
}


//
//	OtRenderPass::end
//

void OtRenderPass::end() {
	OtAssert(open);
	SDL_EndGPURenderPass(pass);
	open = false;
}
