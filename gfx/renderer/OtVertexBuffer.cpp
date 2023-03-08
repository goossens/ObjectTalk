//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtVertexBuffer.h"


//
//	OtVertexBuffer::set
//

void OtVertexBuffer::set(void *data, size_t count, const bgfx::VertexLayout& l) {
	clear();
	layout = l;
	vertexBuffer = bgfx::createVertexBuffer(bgfx::copy(data, layout.getSize((uint32_t) count)), layout);
}


//
//	OtVertexBuffer::submit
//

void OtVertexBuffer::submit(uint8_t stream) {
	if (isValid()) {
		bgfx::setVertexBuffer(stream, vertexBuffer.getHandle());

	} else {
		OtExcept("Internal error: VertexBuffer not initialized before submission");
	}
}
