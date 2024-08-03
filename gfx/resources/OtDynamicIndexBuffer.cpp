//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "OtLog.h"

#include "OtDynamicIndexBuffer.h"


//
//	OtDynamicIndexBuffer::set
//

void OtDynamicIndexBuffer::set(void* data, size_t count) {
	if (!isValid()) {
		indexBuffer = bgfx::createDynamicIndexBuffer(1000);
	}

	bgfx::update(indexBuffer.getHandle(), 0, bgfx::copy(data, uint32_t(count * sizeof(uint32_t))));
	indexCount = count;
}


//
//	OtDynamicIndexBuffer::submit
//

void OtDynamicIndexBuffer::submit() {
	if (isValid()) {
		bgfx::setIndexBuffer(indexBuffer.getHandle(), 0, uint32_t(indexCount));

	} else {
		OtLogFatal("Internal error: dynamicIndexBuffer not initialized before submission");
	}
}
