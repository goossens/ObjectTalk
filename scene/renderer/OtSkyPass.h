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
#include "OtShaderProgram.h"

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
	// render procedural sky
	void renderSky(OtSceneRendererContext& ctx, OtPass& pass, OtSkyComponent& sky);

	// render skybox
	void renderSkyBox(OtSceneRendererContext& ctx, OtPass& pass, OtSkyBoxComponent& skybox);

	// properties
	OtFrameBuffer& framebuffer;

	OtShaderProgram skyProgram{"OtSkyVS", "OtSkyFS"};
	OtShaderProgram skyBoxProgram{"OtSkyVS", "OtSkyBoxFS"};
};
