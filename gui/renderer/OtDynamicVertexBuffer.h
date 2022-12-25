//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	OtDynamicVertexBuffer
//

class OtDynamicVertexBuffer {
public:
	// destructor
	~OtDynamicVertexBuffer();

	// clear the resources
	void clear();

	// set vertices
	void set(void* data, size_t count, const bgfx::VertexLayout& layout);

	// see if buffer is valid
	bool isValid() { return bgfx::isValid(vertexBuffer); }

	// submit to GPU
	void submit(uint8_t stream=0);

private:
	// vertex layout and the actual buffer
	bgfx::VertexLayout layout;
	bgfx::DynamicVertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
};