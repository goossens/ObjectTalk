//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>
#include "OtLog.h"

#include "OtGpu.h"
#include "OtVertexBuffer.h"


//
//	OtVertexBuffer::clear
//

void OtVertexBuffer::clear() {
	vertexBuffer = nullptr;
	transferBuffer = nullptr;
	currentBufferSize = 0;
}


//
//	OtVertexBuffer::set
//

void OtVertexBuffer::set(void* data, size_t count, OtVertexDescription* description, bool dynamic) {
	// remember the vertex description and vertex count
	vertexDescription = description;
	vertexCount = count;

	// create/resize vertex buffer (if required)
	auto size = count * description->size;
	auto& gpu = OtGpu::instance();

	if (!dynamic || size > currentBufferSize) {
		SDL_GPUBufferCreateInfo bufferInfo{};
		bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
		bufferInfo.size = static_cast<Uint32>(size);
		SDL_GPUBuffer* vbuffer = SDL_CreateGPUBuffer(gpu.device, &bufferInfo);

		if (!vbuffer) {
			OtLogFatal("Error in SDL_CreateGPUBuffer: {}", SDL_GetError());
		}

		assignVertexBuffer(vbuffer);

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

	// put vertex data in transfer buffer
	void* bufferData = SDL_MapGPUTransferBuffer(gpu.device, transferBuffer.get(), dynamic);
	std::memcpy(bufferData, data, size);
	SDL_UnmapGPUTransferBuffer(gpu.device, transferBuffer.get());

	// upload vertex buffer to GPU
	SDL_GPUTransferBufferLocation location{};
	location.transfer_buffer = transferBuffer.get();

	SDL_GPUBufferRegion region{};
	region.buffer = vertexBuffer.get();
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
//	OtVertexBuffer::assignVertexBuffer
//

void OtVertexBuffer::assignVertexBuffer(SDL_GPUBuffer* newBuffer) {
	vertexBuffer = std::shared_ptr<SDL_GPUBuffer>(
		newBuffer,
		[](SDL_GPUBuffer* oldBuffer) {
			auto& gpu = OtGpu::instance();
			SDL_ReleaseGPUBuffer(gpu.device, oldBuffer);
			gpu.vertexBuffers--;
		});

	OtGpu::instance().vertexBuffers++;
}


//
//	OtVertexBuffer::assignTransferBuffer
//

void OtVertexBuffer::assignTransferBuffer(SDL_GPUTransferBuffer* newBuffer) {
	transferBuffer = std::shared_ptr<SDL_GPUTransferBuffer>(
		newBuffer,
		[](SDL_GPUTransferBuffer* oldBuffer) {
			SDL_ReleaseGPUTransferBuffer(OtGpu::instance().device, oldBuffer);
		});
}
