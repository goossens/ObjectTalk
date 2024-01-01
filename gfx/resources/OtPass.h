//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"


#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtShaderProgram.h"


//
//	OtPass
//

class OtPass {
public:
	// constructor
	OtPass();

	// manipulate pass
	void setClear(bool color, bool depth, const glm::vec4& rgba=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depthValue=1.0f);
	void setRectangle(int x, int y, int w, int h);
	void setFrameBuffer(OtFrameBuffer& framebuffer);
	void setFrameBuffer(OtGbuffer& gbuffer);
	void setTransform(const glm::mat4& view, const glm::mat4& projection);
	void submitQuad(int w, int h);
	void runShaderProgram(OtShaderProgram& program);
	void touch();

	void blit(
		bgfx::TextureHandle dest,
		uint16_t dx, uint16_t dy,
		bgfx::TextureHandle src,
		uint16_t sx=0, uint16_t sy=0,
		uint16_t sw=UINT16_MAX, uint16_t sh=UINT16_MAX);

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
