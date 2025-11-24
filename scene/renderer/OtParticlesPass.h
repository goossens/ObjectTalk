//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
#include "OtSampler.h"
#include "OtVertexBuffer.h"

#include "OtSceneRendererContext.h"


//
//	OtParticlesPass
//

class OtParticlesPass {
public:
	// constructor
	OtParticlesPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtRenderPipeline pipeline;
	OtSampler particlesSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};

	OtVertexBuffer vertexBuffer;
	OtVertexBuffer particleBuffer;
	OtIndexBuffer indexBuffer;

	// support functions
	void initializeResources();
	bool resourcesInitialized = false;
};
