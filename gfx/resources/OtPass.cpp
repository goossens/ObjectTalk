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

#include "OtLog.h"

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
//	OtPass::OtPass
//

OtPass::OtPass() {
	view = nextViewID++;
	bgfx::resetView(view);
}


//
//	OtPass::setClear
//

void OtPass::setClear(bool color, bool depth, const glm::vec4& rgba, float depthValue) {
	glm::u8vec4 result = glm::u8vec4(glm::round(rgba * 255.0f));
	uint32_t clearColor = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];

	bgfx::setViewClear(
		view,
		(color ? BGFX_CLEAR_COLOR : BGFX_CLEAR_NONE) | (depth ? BGFX_CLEAR_DEPTH : BGFX_CLEAR_NONE),
		clearColor,
		depthValue);
}


//
//	OtPass::setRectangle
//

void OtPass::setRectangle(int x, int y, int w, int h) {
	bgfx::setViewRect(view, (uint16_t) x, (uint16_t) y, (uint16_t) w, (uint16_t) h);
}


//
//	OtPass::setFrameBuffer
//

void OtPass::setFrameBuffer(OtFrameBuffer& framebuffer) {
	framebuffer.submit(view);
}


//
//	OtPass::setFrameBuffer
//

void OtPass::setFrameBuffer(OtGbuffer &gbuffer) {
	gbuffer.submit(view);
}


//
//	OtPass::setTransform
//

void OtPass::setTransform(const glm::mat4 &viewTransform, const glm::mat4 &projection) {
	bgfx::setViewTransform(view, glm::value_ptr(viewTransform), glm::value_ptr(projection));
}


//
//	OtPass::submitQuad
//

void OtPass::submitQuad(int w, int h) {
	// submit a "single triangle" covering the rectangular quad
	if (bgfx::getAvailTransientVertexBuffer(3, OtVertexPosUv::getLayout()) == 3) {
		bool originBottomLeft = OtGpuHasOriginBottomLeft();

		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, OtVertexPosUv::getLayout());
		OtVertexPosUv* vertex = (OtVertexPosUv*) vb.data;

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
		OtLogFatal("Internal error: insufficient transient buffer space");
	}
}


//
//	OtPass::runShaderProgram
//

void OtPass::runShaderProgram(OtShaderProgram& program) {
	program.submit(view);
}


//
//	OtPass::touch
//

void OtPass::touch() {
	bgfx::touch(view);
}


//
//	OtPass::blit
//

void OtPass::blit(bgfx::TextureHandle dest, uint16_t dx, uint16_t dy, bgfx::TextureHandle src, uint16_t sx, uint16_t sy, uint16_t sw, uint16_t sh) {
	bgfx::blit(view, dest, dx, dy, src, sx, sy, sw, sh);
}
