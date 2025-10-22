//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtPass.h"
#include "OtFrameBuffer.h"
#include "OtShaderProgram.h"

#include "OtSceneRendererContext.h"


//
//	OtGridPass
//

class OtGridPass {
public:
	// access properties
	void setGridScale(float gs) { gridScale = gs; }

	// render the pass
	void render(OtSceneRendererContext& ctx, OtFrameBuffer* framebuffer);

private:
	// grid scale (0.0 means no grid)
	float gridScale = 0.0f;

	// properties
	OtShaderProgram program{"OtGridVS", "OtGridFS"};
};
