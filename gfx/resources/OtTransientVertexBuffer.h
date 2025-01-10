//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	OtTransientVertexBuffer
//

class OtTransientVertexBuffer {
public:
	// submit to GPU
	void submit(void* data, size_t count, const bgfx::VertexLayout& layout, uint8_t stream=0);
};
