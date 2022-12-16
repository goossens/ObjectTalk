//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtDynamicVertexBuffer.h"


//
//	OtDynamicVertexBuffer::~OtDynamicVertexBuffer
//

OtDynamicVertexBuffer::~OtDynamicVertexBuffer() {
	clear();
}


//
//	OtDynamicVertexBuffer::clear
//

void OtDynamicVertexBuffer::clear() {
	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
		vertexBuffer = BGFX_INVALID_HANDLE;
	}
}


//
//	OtDynamicVertexBuffer::set
//

void OtDynamicVertexBuffer::set(void *data, size_t count, const bgfx::VertexLayout& l) {
	clear();
	layout = l;
	vertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::copy(data, layout.getSize((uint32_t) count)), layout);
}


//
//	OtDynamicVertexBuffer::update
//

void OtDynamicVertexBuffer::update(void *data, size_t count) {
	bgfx::update(vertexBuffer, 0, bgfx::copy(data, layout.getSize((uint32_t) count)));
}


//
//	OtDynamicVertexBuffer::submit
//

void OtDynamicVertexBuffer::submit(uint8_t stream) {
	bgfx::setVertexBuffer(stream, vertexBuffer);
}
