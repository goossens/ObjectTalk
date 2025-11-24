//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtGbuffer.h"
#include "OtGpu.h"


//
//	OtGbuffer::OtGbuffer
//

OtGbuffer::OtGbuffer() {
	OtGpu::instance().gBuffers++;
}


//
//	OtGbuffer::~OtGbuffer
//

OtGbuffer::~OtGbuffer() {
	OtGpu::instance().gBuffers--;
}


//
//	OtGbuffer::clear
//

void OtGbuffer::clear() {
	// release resources (if required)
	albedoTexture.clear();
	normalTexture.clear();
	pbrTexture.clear();
	emissiveTexture.clear();
	depthTexture.clear();

	// clear other fields
	width = -1;
	height = -1;
	valid = false;
}

bool OtGbuffer::update(int w, int h) {
	// update framebuffer if required
	if (!valid || w != width || h != height) {
		// clear old resources
		clear();

		// create new textures
		auto w32 = static_cast<Uint32>(w);
		auto h32 = static_cast<Uint32>(h);
		auto colorUsage = OtTexture::Usage(OtTexture::Usage::colorTarget | OtTexture::Usage::sampler);
		auto depthUsage = OtTexture::Usage(OtTexture::Usage::depthStencilTarget | OtTexture::Usage::sampler);

		albedoTexture.update(w32, h32, OtTexture::Format::rgba16, colorUsage);
		normalTexture.update(w32, h32, OtTexture::Format::rgba8, colorUsage);
		pbrTexture.update(w32, h32, OtTexture::Format::rgba8, colorUsage);
		emissiveTexture.update(w32, h32, OtTexture::Format::rgba8, colorUsage);
		depthTexture.update(w32, h32, OtTexture::Format::d32, depthUsage);

		// remember dimensions and set state
		width = w;
		height = h;
		valid = true;
		return true;

	} else {
		return false;
	}
}


//
//	OtGbuffer::getRenderTargetInfo
//

OtRenderTargetInfo* OtGbuffer::getRenderTargetInfo(
	bool clearColorTexture,
	bool clearDepthTexture,
	[[maybe_unused]] bool clearStencilTexture,
	glm::vec4 clearColorValue,
	float clearDepthValue,
	[[maybe_unused]] std::uint8_t clearStencilValue) {

	auto color = SDL_FColor{
		clearColorValue.r,
		clearColorValue.g,
		clearColorValue.b,
		clearColorValue.a
	};

	colorTargetInfo[0] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[0].texture = albedoTexture.getTexture();
	colorTargetInfo[0].clear_color = color;
	colorTargetInfo[0].load_op = clearColorTexture ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	colorTargetInfo[0].store_op = SDL_GPU_STOREOP_STORE;

	colorTargetInfo[1] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[1].texture = normalTexture.getTexture();
	colorTargetInfo[1].clear_color = color;
	colorTargetInfo[1].load_op = clearColorTexture ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	colorTargetInfo[1].store_op = SDL_GPU_STOREOP_STORE;

	colorTargetInfo[2] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[2].texture = pbrTexture.getTexture();
	colorTargetInfo[2].clear_color = color;
	colorTargetInfo[2].load_op = clearColorTexture ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	colorTargetInfo[2].store_op = SDL_GPU_STOREOP_STORE;

	colorTargetInfo[3] = SDL_GPUColorTargetInfo{};
	colorTargetInfo[3].texture = emissiveTexture.getTexture();
	colorTargetInfo[3].clear_color = color;
	colorTargetInfo[3].load_op = clearColorTexture ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	colorTargetInfo[3].store_op = SDL_GPU_STOREOP_STORE;

	depthStencilTargetInfo = SDL_GPUDepthStencilTargetInfo{};
	depthStencilTargetInfo.texture = depthTexture.getTexture();
	depthStencilTargetInfo.clear_depth = clearDepthValue,
	depthStencilTargetInfo.load_op = clearDepthTexture ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

	info.colorTargetInfo = colorTargetInfo;
	info.numColorTargets = 4;
	info.depthStencilTargetInfo = &depthStencilTargetInfo;

	return &info;
}
