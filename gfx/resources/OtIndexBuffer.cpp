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

#include "OtGpu.h"
#include "OtIndexBuffer.h"


//
//	OtIndexBuffer::clear
//

void OtIndexBuffer::clear() {
	indexBuffer = nullptr;
	transferBuffer = nullptr;
	currentBufferSize = 0;
}


//
//	OtIndexBuffer::set
//

void OtIndexBuffer::set(uint32_t* data, size_t count, bool dynamic) {
	// remember index count
	indexCount = count;

	// create/resize index buffer (if required)
	auto size = count * sizeof(uint32_t);
	auto& gpu = OtGpu::instance();

	if (!dynamic || size > currentBufferSize) {
		SDL_GPUBufferCreateInfo bufferInfo{};
		bufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
		bufferInfo.size = static_cast<Uint32>(size);
		SDL_GPUBuffer* ibuffer = SDL_CreateGPUBuffer(gpu.device, &bufferInfo);

		if (!ibuffer) {
			OtLogFatal("Error in SDL_CreateGPUBuffer: {}", SDL_GetError());
		}

		assignIndexBuffer(ibuffer);

		// create a transfer buffer
		SDL_GPUTransferBufferCreateInfo transferInfo{};
		transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		transferInfo.size = static_cast<Uint32>(size);

		SDL_GPUTransferBuffer* tbuffer = SDL_CreateGPUTransferBuffer(gpu.device, &transferInfo);

		if (!tbuffer) {
			OtLogFatal("Error in SDL_CreateGPUTransferBuffer: {}", SDL_GetError());
		}

		assignTransferBuffer(tbuffer);
	}

	// put index data in transfer buffer
	void* bufferData = SDL_MapGPUTransferBuffer(gpu.device, transferBuffer.get(), dynamic);
	std::memcpy(bufferData, data, size);
	SDL_UnmapGPUTransferBuffer(gpu.device, transferBuffer.get());

	// upload index buffer to GPU
	SDL_GPUTransferBufferLocation location{};
	location.transfer_buffer = transferBuffer.get();

	SDL_GPUBufferRegion region{};
	region.buffer = indexBuffer.get();
	region.size = static_cast<Uint32>(size);

	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(gpu.copyCommandBuffer);
	SDL_UploadToGPUBuffer(copyPass, &location, &region, dynamic);
	SDL_EndGPUCopyPass(copyPass);

	if (dynamic) {
		currentBufferSize = size;

	} else {
		transferBuffer = nullptr;
		currentBufferSize = 0;
	}
}


//
//	OtIndexBuffer::assignIndexBuffer
//

void OtIndexBuffer::assignIndexBuffer(SDL_GPUBuffer* newBuffer) {
	indexBuffer = std::shared_ptr<SDL_GPUBuffer>(
		newBuffer,
		[](SDL_GPUBuffer* oldBuffer) {
			auto& gpu = OtGpu::instance();
			SDL_ReleaseGPUBuffer(gpu.device, oldBuffer);
			gpu.indexBuffers--;
		});

	OtGpu::instance().indexBuffers++;
}


//
//	OtIndexBuffer::assignTransferBuffer
//

void OtIndexBuffer::assignTransferBuffer(SDL_GPUTransferBuffer* newBuffer) {
	transferBuffer = std::shared_ptr<SDL_GPUTransferBuffer>(
		newBuffer,
		[](SDL_GPUTransferBuffer* oldBuffer) {
			SDL_ReleaseGPUTransferBuffer(OtGpu::instance().device, oldBuffer);
		});
}
