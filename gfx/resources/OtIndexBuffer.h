//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <memory>

#include "SDL3/SDL.h"


//
//	OtIndexBuffer
//

class OtIndexBuffer {
public:
	// clear the object
	void clear();

	// see if buffer is valid
	inline bool isValid() { return indexBuffer != nullptr; }

	// set indices
	void set(uint32_t* data, size_t count, bool dynamic=false);

	// get index count
	inline size_t getCount() { return indexCount; }

private:
	// the GPU resource
	std::shared_ptr<SDL_GPUBuffer> indexBuffer;
	std::shared_ptr<SDL_GPUTransferBuffer> transferBuffer;

	// memory manage SDL resource
	void assignIndexBuffer(SDL_GPUBuffer* newBuffer);
	void assignTransferBuffer(SDL_GPUTransferBuffer* newBuffer);

	// number of indices
	size_t indexCount = 0;

	// current dynamic buffer size
	size_t currentBufferSize = 0;

	// get accesss to the raw buffer handle
	friend class OtRenderPass;
	inline SDL_GPUBuffer* getBuffer() { return indexBuffer.get(); }
};
