//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtFrameBuffer.h"
#include "OtPostProcessing.h"
#include "OtRenderLight.h"
#include "OtTexture.h"

#include "OtSceneRendererContext.h"

#include "OtBlitPass.h"
#include "OtBloomDownSample.h"
#include "OtBloomUpSample.h"
#include "OtCompositingAdd.h"
#include "OtGodRays.h"
#include "OtFog.h"
#include "OtFxaa.h"
#include "OtOcclusionPass.h"


//
//	OtPostProcessingPass
//

class OtPostProcessingPass {
public:
	// constructor
	OtPostProcessingPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	OtTexture* render(OtSceneRendererContext& ctx);

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// properties
	OtFrameBuffer& framebuffer;
	OtTexture postProcessBuffer1;
	OtTexture postProcessBuffer2;
	OtTexture occlusionBuffer;

	OtRenderLight renderLight;
	OtOcclusionPass occlusionPass{occlusionBuffer};
	OtGodRays godRays;

	static constexpr int bloomDepth = 5;
	OtTexture bloomBuffer[bloomDepth];

	OtFxaa fxaa;
	OtFog fog;
	OtBloomDownSample bloomDownSample;
	OtBloomUpSample bloomUpSample;
	OtCompositingAdd compositingAdd;
	OtPostProcessing postprocess;
};
