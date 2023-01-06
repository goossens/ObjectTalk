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
//	OtIndexBuffer
//

class OtIndexBuffer {
public:
	// destructor
	~OtIndexBuffer();

	// clear the resources
	void clear();

	// set indices
	void set(uint32_t* data, size_t count);

	// see if buffer is valid
	bool isValid() { return bgfx::isValid(indexBuffer); }

	// submit to GPU
	void submit();

private:
	// the actual buffer
	bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;
};