//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtShader.h"


//
//	OtPass
//

class OtPass {
public:
	// reserve a rendering slot for this pass
	void reserveRenderingSlot();

	// set properties
	void setClear(bool color, bool depth);
	void setRectangle(int x, int y, int w, int h);
	void setFrameBuffer(OtFrameBuffer& framebuffer);
	void setTransform(const glm::mat4& view, const glm::mat4& projection);
	void runShader(OtShader& shader);

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
