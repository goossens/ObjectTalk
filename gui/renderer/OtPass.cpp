//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OtException.h"

#include "OtGpu.h"
#include "OtPass.h"
#include "OtVertex.h"


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

void OtPass::setClear(bool color, bool depth, uint32_t rgba, float depthValue) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	bgfx::setViewClear(
		view,
		(color ? BGFX_CLEAR_COLOR : BGFX_CLEAR_NONE) | (depth ? BGFX_CLEAR_DEPTH : BGFX_CLEAR_NONE),
		rgba,
		depthValue);
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
//	OtPass::setFrameBuffer
//

void OtPass::setFrameBuffer(OtGbuffer &gbuffer) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	gbuffer.submit(view);
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


//submitQuad
//	OtPass::setQuad
//

void OtPass::submitQuad(int w, int h) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	// submit a "single triangle" covering the rectangular quad
	if (bgfx::getAvailTransientVertexBuffer(3, OtVertexPosTex::getLayout()) == 3) {
		bool originBottomLeft = OtGpuHasOriginBottomLeft();

		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, OtVertexPosTex::getLayout());
		OtVertexPosTex* vertex = (OtVertexPosTex*) vb.data;

		vertex[0].position = glm::vec3(-w, 0.0, 0.0);
		vertex[0].uv = originBottomLeft ? glm::vec2(-1.0, 1.0) : glm::vec2(-1.0, 0.0);

		vertex[1].position = glm::vec3(w, 0.0, 0.0);
		vertex[1].uv = originBottomLeft ? glm::vec2(1.0, 1.0) : glm::vec2(1.0, 0.0);

		vertex[2].position = glm::vec3(w, h * 2.0, 0.0);
		vertex[2].uv = originBottomLeft ? glm::vec2(1.0, -1.0) : glm::vec2(1.0, 2.0);

		bgfx::setVertexBuffer(0, &vb);
		bgfx::setViewRect(view, 0, 0, (uint16_t) w, (uint16_t) h);
		bgfx::setViewClear(view, BGFX_CLEAR_NONE, BGFX_CLEAR_NONE);

		glm::mat4 projMatrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
		bgfx::setViewTransform(view, nullptr, glm::value_ptr(projMatrix));

	} else {
		OtExcept("Internal error: insufficient transient buffer space");
	}
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


//
//	OtPass::blit
//

void OtPass::blit(bgfx::TextureHandle dest, uint16_t dx, uint16_t dy, bgfx::TextureHandle src, uint16_t sx, uint16_t sy, uint16_t sw, uint16_t sh) {
	if (!view) {
		OtExcept("Internal error: rendering slot for pass not reserved");
	}

	bgfx::blit(view, dest, dx, dy, src, sx, sy, sw, sh);
}
