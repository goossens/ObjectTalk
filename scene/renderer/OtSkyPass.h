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
#include "OtRenderPass.h"
#include "OtRenderPipeline.h"

#include "OtSceneRendererContext.h"
#include "OtSkyBoxComponent.h"
#include "OtSkyComponent.h"


//
//	OtSkyPass
//

class OtSkyPass {
public:
	// constructor
	OtSkyPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtRenderPipeline skyPipeline;
	OtRenderPipeline skyBoxPipeline;

	// support functions
	void renderSky(OtSceneRendererContext& ctx, OtSkyComponent& sky);
	void renderSkyBox(OtSceneRendererContext& ctx, OtSkyBoxComponent& skybox);

	void initializeResources();
	bool resourcesInitialized = false;
};
