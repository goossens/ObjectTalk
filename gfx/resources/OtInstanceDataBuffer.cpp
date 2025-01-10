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

#include "OtInstanceDataBuffer.h"


//
//	OtInstanceDataBuffer::submit
//

void OtInstanceDataBuffer::submit(void* data, size_t count, size_t stride) {
	// sanity check
	if ((bgfx::getAvailInstanceDataBuffer((uint32_t) count, (uint32_t) stride) != count)) {
		OtLogFatal("Internal error: insufficient instance data buffer space");
	}

	// create instance data buffer and submit it to the GPU
	bgfx::InstanceDataBuffer idb;
	bgfx::allocInstanceDataBuffer(&idb, (uint32_t) count, (uint32_t) stride);
	std::memcpy(idb.data, data, idb.size);
	bgfx::setInstanceDataBuffer(&idb);
}
