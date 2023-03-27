//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtDynamicIndexBuffer.h"


//
//	OtDynamicIndexBuffer::set
//

void OtDynamicIndexBuffer::set(void *data, size_t count) {
	if (isValid()) {
		bgfx::update(indexBuffer.getHandle(), 0, bgfx::copy(data, (uint32_t) (count * sizeof(uint32_t))));

	} else {
		indexBuffer = bgfx::createDynamicIndexBuffer(bgfx::copy(data, (uint32_t) (count * sizeof(uint32_t))), BGFX_BUFFER_INDEX32);
	}
}


//
//	OtDynamicIndexBuffer::submit
//

void OtDynamicIndexBuffer::submit() {
	if (isValid()) {
		bgfx::setIndexBuffer(indexBuffer.getHandle());

	} else {
		OtError("Internal error: DynamicIndexBuffer not initialized before submission");
	}
}
