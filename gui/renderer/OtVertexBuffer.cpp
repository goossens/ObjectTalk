//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtVertexBuffer.h"


//
//	OtVertexBuffer::~OtVertexBuffer
//

OtVertexBuffer::~OtVertexBuffer() {
	clear();
}


//
//	OtVertexBuffer::clear
//

void OtVertexBuffer::clear() {
	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
		vertexBuffer = BGFX_INVALID_HANDLE;
	}
}


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
	bgfx::setVertexBuffer(stream, vertexBuffer);
}
