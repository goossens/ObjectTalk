//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	indexBuffer = bgfx::createIndexBuffer(bgfx::copy(data, (uint32_t) (count * sizeof(uint32_t))), BGFX_BUFFER_INDEX32);
}


//
//	OtIndexBuffer::submit
//

void OtIndexBuffer::submit() {
	if (isValid()) {
		bgfx::setIndexBuffer(indexBuffer.getHandle());

	} else {
		OtLogFatal("Internal error: IndexBuffer not initialized before submission");
	}
}
