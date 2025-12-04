//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "SDL3/SDL.h"

#include "OtVertex.h"


//
//	OtVertexBuffer
//

class OtVertexBuffer {
public:
	// clear the object
	void clear();

	// see if buffer is valid
	inline bool isValid() { return vertexBuffer != nullptr; }

	// set vertices
	void set(void* data, size_t count, OtVertexDescription* description, bool dynamic=false);

	// get vertex description and count
	inline OtVertexDescription* getDescription() { return vertexDescription; }
	inline size_t getCount() { return vertexCount; }

private:
	// the GPU resources
	std::shared_ptr<SDL_GPUBuffer> vertexBuffer;
	std::shared_ptr<SDL_GPUTransferBuffer> transferBuffer;

	// memory manage SDL resource
	void assignVertexBuffer(SDL_GPUBuffer* newBuffer);
	void assignTransferBuffer(SDL_GPUTransferBuffer* newBuffer);

	// vertex description and count
	OtVertexDescription* vertexDescription = nullptr;
	size_t vertexCount = 0;

	// current dynamic buffer size
	size_t currentBufferSize = 0;

	// get accesss to the raw buffer handle
	friend class OtRenderPass;
	inline SDL_GPUBuffer* getBuffer() { return vertexBuffer.get(); }
};
