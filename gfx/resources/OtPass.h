//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"


#include "OtComputeProgram.h"
#include "OtCubeMap.h"
#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"


//
//	OtPass
//

class OtPass {
public:
	// access flags
	static constexpr int readAccess = bgfx::Access::Read;
	static constexpr int writeAccess = bgfx::Access::Write;
	static constexpr int readWriteAccess = bgfx::Access::ReadWrite;

	// constructor
	OtPass();

	// handle rendering pass
	void setClear(bool color, bool depth=false, const glm::vec4& rgba=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depthValue=1.0f);
	void setRectangle(int x, int y, int w, int h);
	void setFrameBuffer(bgfx::FrameBufferHandle framebuffer);
	void setFrameBuffer(OtFrameBuffer& framebuffer);
	void setFrameBuffer(OtGbuffer& gbuffer);
	void setTransform(const glm::mat4& view, const glm::mat4& projection);
	void submitQuad(int w, int h);
	void submitCircle(int segments);
	void submitCube();
	void runShaderProgram(OtShaderProgram& program);
	void touch();

	// handle compute pass
	void setImage(int stage, OtTexture& texture, int mip, int access);
	void setImage(int stage, OtCubeMap& cubemap, int mip, int access);
	void runComputeProgram(OtComputeProgram& program, uint32_t x, uint32_t y, uint32_t z);

	// handle blit pass
	void blit(
		bgfx::TextureHandle dest,
		uint16_t dx, uint16_t dy,
		bgfx::TextureHandle src,
		uint16_t sx=0, uint16_t sy=0,
		uint16_t sw=UINT16_MAX, uint16_t sh=UINT16_MAX);

	// get view ID
	inline bgfx::ViewId getViewId() { return view; }

private:
	// properties
	bgfx::ViewId view = 0;
};


//
//	Functions
//

// reset rendering pass system (to be run each frame)
void OtPassReset();

// get view count
int OtPassCount();
