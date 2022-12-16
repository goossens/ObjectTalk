//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtIndexBuffer.h"


//
//	OtIndexBuffer::~OtIndexBuffer
//

OtIndexBuffer::~OtIndexBuffer() {
	clear();
}


//
//	OtIndexBuffer::clear
//

void OtIndexBuffer::clear() {
	if (bgfx::isValid(indexBuffer)) {
		bgfx::destroy(indexBuffer);
		indexBuffer = BGFX_INVALID_HANDLE;
	}
}


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
	bgfx::setIndexBuffer(indexBuffer);
}
