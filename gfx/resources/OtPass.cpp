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
//	OtPass::submitQuad
//

void OtPass::submitQuad(int w, int h) {
	// submit a "single triangle" covering the rectangular quad
	bool originBottomLeft = OtGpuHasOriginBottomLeft();
	OtVertexPosUv vertices[3];

	vertices[0].position = glm::vec3(static_cast<float>(-w), 0.0f, 0.0f);
	vertices[0].uv = originBottomLeft ? glm::vec2(-1.0f, 1.0f) : glm::vec2(-1.0f, 0.0f);

	vertices[1].position = glm::vec3(w, 0.0, 0.0);
	vertices[1].uv = originBottomLeft ? glm::vec2(1.0f, 1.0f) : glm::vec2(1.0f, 0.0f);

	vertices[2].position = glm::vec3(static_cast<float>(w), static_cast<float>(h) * 2.0f, 0.0f);
	vertices[2].uv = originBottomLeft ? glm::vec2(1.0f, -1.0f) : glm::vec2(1.0f, 2.0f);

	OtTransientVertexBuffer tvb;
	tvb.submit(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPosUv::getLayout());

	bgfx::setViewRect(view, 0, 0, static_cast<uint16_t>(w), static_cast<uint16_t>(h));
	bgfx::setViewClear(view, BGFX_CLEAR_NONE, BGFX_CLEAR_NONE);

	glm::mat4 projMatrix = glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(projMatrix));
}
