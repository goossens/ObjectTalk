//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtIndexBuffer.h"


//
//	OtIndexBuffer::setVertices
//

void OtIndexBuffer::set(uint32_t* data, size_t count) {
	clear();
	indexBuffer = bgfx::createIndexBuffer(bgfx::copy(data, static_cast<uint32_t>((count * sizeof(uint32_t)))), BGFX_BUFFER_INDEX32);
}


//
//	OtIndexBuffer::submit
//

void OtIndexBuffer::submit() {
	if (isValid()) {
		bgfx::setIndexBuffer(indexBuffer.getHandle());

	} else {
		OtLogFatal("Internal error: indexBuffer not initialized before submission");
	}
}
