//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtDynamicIndexBuffer.h"


//
//	OtDynamicIndexBuffer::~OtDynamicIndexBuffer
//

OtDynamicIndexBuffer::~OtDynamicIndexBuffer() {
	clear();
}


//
//	OtDynamicIndexBuffer::clear
//

void OtDynamicIndexBuffer::clear() {
	if (bgfx::isValid(indexBuffer)) {
		bgfx::destroy(indexBuffer);
		indexBuffer = BGFX_INVALID_HANDLE;
	}
}


//
//	OtDynamicIndexBuffer::set
//

void OtDynamicIndexBuffer::set(void *data, size_t count) {
	clear();
	indexBuffer = bgfx::createDynamicIndexBuffer(bgfx::copy(data, (uint32_t) (count * sizeof(uint32_t))), BGFX_BUFFER_INDEX32);
}


//
//	OtDynamicIndexBuffer::update
//

void OtDynamicIndexBuffer::update(void *data, size_t count) {
	bgfx::update(indexBuffer, 0, bgfx::copy(data, (uint32_t) (count * sizeof(uint32_t))));
}


//
//	OtDynamicIndexBuffer::submit
//

void OtDynamicIndexBuffer::submit() {
	bgfx::setIndexBuffer(indexBuffer);
}
