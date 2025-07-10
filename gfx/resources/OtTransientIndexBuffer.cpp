//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bgfx/bgfx.h"

#include "OtLog.h"

#include "OtTransientIndexBuffer.h"


//
//	OtTransientIndexBuffer::submit
//

void OtTransientIndexBuffer::submit(uint32_t* data, size_t count) {
	// sanity check
	if ((bgfx::getAvailTransientIndexBuffer(static_cast<uint32_t>(count)) != count)) {
		OtLogFatal("Internal error: insufficient transient index buffer space");
	}

	// allocate transient space, fill buffer and submit to GPU
	bgfx::TransientIndexBuffer tib;
	bgfx::allocTransientIndexBuffer(&tib, static_cast<uint32_t>(count), true);
	std::memcpy(tib.data, data, tib.size);
	bgfx::setIndexBuffer(&tib);
}
