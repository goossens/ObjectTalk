//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"

#include "OtException.h"

#include "OtPass.h"


//
//	Globals
//

static bgfx::ViewId nextViewID = 1;


//
//	OtPassReset
//

void OtPassReset() {
	nextViewID = 1;
}


//
//	OtPassCount
//

int OtPassCount() {
	return nextViewID - 1;
}


//
//	OtPass::reserveRenderingSlot
//

void OtPass::reserveRenderingSlot() {
	view = nextViewID++;
	bgfx::resetView(view);
	bgfx::touch(view);
}


//
//	OtPass::setClear
//

void OtPass::setClear(bool color, bool depth) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	bgfx::setViewClear(
		view,
		(color ? BGFX_CLEAR_COLOR : BGFX_CLEAR_NONE) | (depth ? BGFX_CLEAR_DEPTH : BGFX_CLEAR_NONE));
}


//
//	OtPass::setRectangle
//

void OtPass::setRectangle(int x, int y, int w, int h) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	bgfx::setViewRect(view, (uint16_t) x, (uint16_t) y, (uint16_t) w, (uint16_t) h);
}

//
//	OtPass::setFrameBuffer
//

void OtPass::setFrameBuffer(OtFrameBuffer& framebuffer) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	framebuffer.submit(view);
}


//
//	OtPass::setTransform
//

void OtPass::setTransform(const glm::mat4 &viewTransform, const glm::mat4 &projection) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	bgfx::setViewTransform(view, glm::value_ptr(viewTransform), glm::value_ptr(projection));
}


//
//	OtPass::runShader
//

void OtPass::runShader(OtShader &shader) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	shader.submit(view);
}
