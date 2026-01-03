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
#include "OtIndexBuffer.h"
#include "OtRenderPipeline.h"
#include "OtVertexBuffer.h"

#include "OtSceneRendererContext.h"


//
//	OtGridPass
//

class OtGridPass {
public:
	// constructor
	OtGridPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// access properties
	void setGridScale(float gs) { gridScale = gs; }

	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// grid scale (0.0 means no grid)
	float gridScale = 0.0f;

	// properties
	OtFrameBuffer& framebuffer;
	OtRenderPipeline pipeline;
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;

	// support functions
	void initializeResources();
	bool resourcesInitialized = false;
};
