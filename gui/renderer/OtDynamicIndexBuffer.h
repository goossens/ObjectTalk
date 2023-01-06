//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	OtDynamicIndexBuffer
//

class OtDynamicIndexBuffer {
public:
	// destructor
	~OtDynamicIndexBuffer();

	// clear the resources
	void clear();

	// set vertices
	void set(void* data, size_t count);

	// see if buffer is valid
	bool isValid() { return bgfx::isValid(indexBuffer); }

	// submit to GPU
	void submit();

private:
	// the actual buffer
	bgfx::DynamicIndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;
};