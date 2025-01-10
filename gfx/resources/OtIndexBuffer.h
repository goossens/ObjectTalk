//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBgfxHandle.h"


//
//	OtIndexBuffer
//

class OtIndexBuffer {
public:
	// clear the resources
	inline void clear() { indexBuffer.clear(); }

	// set indices
	void set(uint32_t* data, size_t count);

	// see if buffer is valid
	inline bool isValid() { return indexBuffer.isValid(); }

	// submit to GPU
	void submit();

private:
	// the actual buffer
	OtBgfxHandle<bgfx::IndexBufferHandle> indexBuffer;
};