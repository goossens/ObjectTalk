//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "OtLog.h"

#include "OtTransientVertexBuffer.h"


//
//	OtTransientVertexBuffer::submit
//

void OtTransientVertexBuffer::submit(void* data, size_t count, const bgfx::VertexLayout& layout, uint8_t stream) {
	// sanity check
	if ((bgfx::getAvailTransientVertexBuffer((uint32_t) count, layout) != count)) {
		OtLogFatal("Internal error: insufficient transient vertex buffer space");
	}

	// allocate transient space and fill buffer
	bgfx::TransientVertexBuffer tvb;
	bgfx::allocTransientVertexBuffer(&tvb, (uint32_t) count, layout);
	std::memcpy(tvb.data, data, tvb.size);
	bgfx::setVertexBuffer(stream, &tvb);
}
