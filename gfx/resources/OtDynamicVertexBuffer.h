//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBgfxHandle.h"


//
//	OtDynamicVertexBuffer
//

class OtDynamicVertexBuffer {
public:
	// constructors
	OtDynamicVertexBuffer() = default;
	OtDynamicVertexBuffer(size_t ms) : maxSize(ms) {}

	// clear the resources
	inline void clear() { vertexBuffer.clear(); }

	// set size
	inline void setMaxSize(size_t ms) { maxSize = ms; }

	// set vertices
	void set(void* data, size_t count, const bgfx::VertexLayout& layout);

	// see if buffer is valid
	inline bool isValid() { return vertexBuffer.isValid(); }

	// submit to GPU
	void submit(uint8_t stream=0);

private:
	// vertex layout and the actual buffer
	bgfx::VertexLayout layout;
	OtBgfxHandle<bgfx::DynamicVertexBufferHandle> vertexBuffer;
	size_t maxSize = 20000;
	size_t vertexCount;
};