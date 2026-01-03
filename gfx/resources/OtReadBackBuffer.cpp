//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>

#include "SDL3/SDL.h"

#include "OtGpu.h"
#include "OtReadBackBuffer.h"


//
//	OtReadBackBuffer::clear
//

void OtReadBackBuffer::clear() {
	// clear the image
	image.clear();
}


//
//	OtReadBackBuffer::readback
//

OtImage& OtReadBackBuffer::readback(OtTexture& texture) {
	return readback(texture, 0, 0, texture.getWidth(), texture.getHeight());
}


//
//	OtReadBackBuffer::readback
//

OtImage& OtReadBackBuffer::readback(OtTexture& texture, int x, int y, int w, int h) {
	// this is a hack to ensure things happen in the right order
	// this object creates it's own command buffer which is executed immediately
	// therefore, previous GPU commands have to be executed first
	auto& gpu = OtGpu::instance();
	gpu.flushAndRestartFrame();

	// create a transfer buffer
	SDL_GPUTransferBufferCreateInfo bufferInfo{};
	bufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
	bufferInfo.size = static_cast<Uint32>(texture.getBpp() * w * h);

	// transfer GPU texture back to CPU
	auto transferBuffer = SDL_CreateGPUTransferBuffer(gpu.device, &bufferInfo);

	SDL_GPUTextureRegion region{};
	region.texture = texture.getTexture();
	region.x = static_cast<Uint32>(x);
	region.y = static_cast<Uint32>(y);
	region.w = static_cast<Uint32>(w);
	region.h = static_cast<Uint32>(h);
	region.d = 1;

	SDL_GPUTextureTransferInfo transferInfo{};
	transferInfo.transfer_buffer = transferBuffer;
	transferInfo.offset = 0;
	transferInfo.pixels_per_row = 0;
	transferInfo.rows_per_layer = 0;

	auto commandBuffer = SDL_AcquireGPUCommandBuffer(gpu.device);
	auto copyPass = SDL_BeginGPUCopyPass(commandBuffer);
	SDL_DownloadFromGPUTexture(copyPass, &region, &transferInfo);
	SDL_EndGPUCopyPass(copyPass);

	// wait for download to complete
	auto fence = SDL_SubmitGPUCommandBufferAndAcquireFence(commandBuffer);
	SDL_WaitForGPUFences(gpu.device, true, &fence, 1);
	SDL_ReleaseGPUFence(gpu.device, fence);

	// transfer data to image
	auto data = SDL_MapGPUTransferBuffer(gpu.device, transferBuffer, false);
	convertToImage(w, h, texture.getFormat(), data);
	SDL_UnmapGPUTransferBuffer(gpu.device, transferBuffer);
	SDL_ReleaseGPUTransferBuffer(gpu.device, transferBuffer);

	// return reference to image
	return image;
}


//
//	OtReadBackBuffer::convertToImage
//

void OtReadBackBuffer::convertToImage(int w, int h, OtTexture::Format format, void* data) {
	if (format == OtTexture::Format::r8) {
		image.load(w, h, OtImage::Format::r8, data);

	} else if (format == OtTexture::Format::rgba8) {
		image.load(w, h, OtImage::Format::rgba8, data);

	} else if (format == OtTexture::Format::rgba32) {
		image.load(w, h, OtImage::Format::rgba32, data);

	} else if (format == OtTexture::Format::rg16) {
		auto buffer = new glm::vec4[w * h];
		auto src = (uint16_t*) data;
		auto dst = &buffer[0];

		for (int i = 0; i < w * h; i++) {
			auto red = static_cast<float>(*src++) / 255.0f;
			auto green = static_cast<float>(*src++) / 255.0f;
			*dst++ = glm::vec4(red, green, 0.0f, 1.0f);
		}

		image.load(w, h, OtImage::Format::rgba32, buffer);
		delete [] buffer;

	} else if (format == OtTexture::Format::r32) {
		auto buffer = new glm::vec4[w * h];
		auto src = (float*) data;
		auto dst = &buffer[0];

		for (int i = 0; i < w * h; i++) {
			*dst++ = glm::vec4(*src++, 0.0f, 0.0f, 1.0f);
		}

		image.load(w, h, OtImage::Format::rgba32, buffer);
		delete [] buffer;

	} else {
		OtLogFatal("Texture type can't be read back to image");
	}
}
