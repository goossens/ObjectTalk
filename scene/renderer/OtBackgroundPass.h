//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"

#include "OtSceneRendererContext.h"


//
//	OtBackgroundPass
//

class OtBackgroundPass {
public:
	// constructor
	OtBackgroundPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// properties
	OtFrameBuffer& framebuffer;
};
