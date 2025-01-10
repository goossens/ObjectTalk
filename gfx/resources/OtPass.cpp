//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <cmath>
#include <cstring>
#include <vector>

#include "OtNumbers.h"

#include "OtGpu.h"
#include "OtPass.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
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

	// without this call, the default is (view, BGFX_CLEAR_NONE, 0, 0.0f, 0)
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

void OtPass::setFrameBuffer(bgfx::FrameBufferHandle framebuffer) {
	bgfx::setViewFrameBuffer(view, framebuffer);
}

void OtPass::setFrameBuffer(OtFrameBuffer& framebuffer) {
	framebuffer.submit(view);
}

void OtPass::setFrameBuffer(OtGbuffer& gbuffer) {
	gbuffer.submit(view);
}


//
//	OtPass::setTransform
//

void OtPass::setTransform(const glm::mat4& viewTransform, const glm::mat4& projectionTransform) {
	bgfx::setViewTransform(view, glm::value_ptr(viewTransform), glm::value_ptr(projectionTransform));
}


//
//	OtPass::setViewMode
//

void OtPass::setViewMode(int mode) {
	bgfx::setViewMode(view, bgfx::ViewMode::Enum(mode));
}


//
//	OtPass::submitQuad
//

void OtPass::submitQuad(int w, int h) {
	// submit a "single triangle" covering the rectangular quad
	bool originBottomLeft = OtGpuHasOriginBottomLeft();
	OtVertexPosUv vertices[3];

	vertices[0].position = glm::vec3(-w, 0.0, 0.0);
	vertices[0].uv = originBottomLeft ? glm::vec2(-1.0, 1.0) : glm::vec2(-1.0, 0.0);

	vertices[1].position = glm::vec3(w, 0.0, 0.0);
	vertices[1].uv = originBottomLeft ? glm::vec2(1.0, 1.0) : glm::vec2(1.0, 0.0);

	vertices[2].position = glm::vec3(w, h * 2.0, 0.0);
	vertices[2].uv = originBottomLeft ? glm::vec2(1.0, -1.0) : glm::vec2(1.0, 2.0);

	OtTransientVertexBuffer tvb;
	tvb.submit(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPosUv::getLayout());

	bgfx::setViewRect(view, 0, 0, (uint16_t) w, (uint16_t) h);
	bgfx::setViewClear(view, BGFX_CLEAR_NONE, BGFX_CLEAR_NONE);

	glm::mat4 projMatrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(projMatrix));
}


//
//	OtPass::submitCircle
//

void OtPass::submitCircle(int segments) {
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	float segment = std::numbers::pi2 / segments;

	for (auto i = 0; i < segments; i++) {
		float angle = segment * i;
		vertices.emplace_back(glm::vec3(std::cos(angle), std::sin(angle), 0.0f));
	}

	for (auto i = 0; i < segments - 2; i++) {
		indices.emplace_back(0);
		indices.emplace_back(i + 1);
		indices.emplace_back(i + 2);
	}

	OtTransientVertexBuffer tvb;
	tvb.submit(vertices.data(), vertices.size(), OtVertexPos::getLayout());

	OtTransientIndexBuffer tib;
	tib.submit(indices.data(), indices.size());
}


//
//	OtPass::submitCube
//

void OtPass::submitCube() {
	static float vertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// submit the cube geometry
	OtTransientVertexBuffer tvb;
	tvb.submit(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPos::getLayout());
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
//	OtPass::setImage
//

void OtPass::setImage(int stage, OtTexture& texture, int mip, int access) {
	bgfx::setImage(stage, texture.getHandle(), mip, bgfx::Access::Enum(access));
}

void OtPass::setImage(int stage, OtCubeMap& cubemap, int mip, int access) {
	bgfx::setImage(stage, cubemap.getHandle(), mip, bgfx::Access::Enum(access));
}


//
//	OtPass::runComputeProgram
//

void OtPass::runComputeProgram(OtComputeProgram& program, uint32_t x, uint32_t y, uint32_t z) {
	program.dispatch(view, x, y, z);
}


//
//	OtPass::blit
//

void OtPass::blit(bgfx::TextureHandle dest, uint16_t dx, uint16_t dy, bgfx::TextureHandle src, uint16_t sx, uint16_t sy, uint16_t sw, uint16_t sh) {
	bgfx::blit(view, dest, dx, dy, src, sx, sy, sw, sh);
}
