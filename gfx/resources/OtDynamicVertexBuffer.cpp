//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtDynamicVertexBuffer.h"


//
//	OtDynamicVertexBuffer::set
//

void OtDynamicVertexBuffer::set(void* data, size_t count, const bgfx::VertexLayout& l) {
 	if (layout.m_hash != l.m_hash) {
		clear();
	}

	if (isValid()) {
		bgfx::update(vertexBuffer.getHandle(), 0, bgfx::copy(data, layout.getSize((uint32_t) count)));

	} else {
		layout = l;
		vertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::copy(data, layout.getSize((uint32_t) count)), layout);
	}
}


//
//	OtDynamicVertexBuffer::submit
//

void OtDynamicVertexBuffer::submit(uint8_t stream) {
	if (isValid()) {
		bgfx::setVertexBuffer(stream, vertexBuffer.getHandle());

	} else {
		OtLogFatal("Internal error: DynamicVertexBuffer not initialized before submission");
	}
}
