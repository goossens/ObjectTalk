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
#include "OtGbuffer.h"
#include "OtShaderProgram.h"

#include "OtBackgroundPass.h"
#include "OtDeferredPass.h"
#include "OtForwardPass.h"
#include "OtSkyPass.h"


//
//	OtWaterPass
//

class OtWaterPass {
public:
	// constructor
	OtWaterPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// support functions
	void renderReflection(OtSceneRendererContext& ctx);
	void renderRefraction(OtSceneRendererContext& ctx);
	void renderWater(OtSceneRendererContext& ctx, OtWaterComponent& water);

	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// properties
	OtFrameBuffer& framebuffer;

	OtGbuffer renderingBuffer;
	OtFrameBuffer reflectionBuffer{OtTexture::rgbaFloat16Texture, OtTexture::dFloatTexture};
	OtFrameBuffer refractionBuffer{OtTexture::rgbaFloat16Texture, OtTexture::dFloatTexture};

	OtShaderProgram waterProgram{"OtWaterVS", "OtWaterFS"};

	OtBackgroundPass backgroundReflectionPass{reflectionBuffer};
	OtDeferredPass deferredReflectionPass{renderingBuffer, reflectionBuffer};
	OtForwardPass forwardReflectionPass{reflectionBuffer};
	OtSkyPass skyReflectionPass{reflectionBuffer};

	OtBackgroundPass backgroundRefractionPass{refractionBuffer};
	OtDeferredPass deferredRefractionPass{renderingBuffer, refractionBuffer};
	OtForwardPass forwardRefractionPass{refractionBuffer};
	OtSkyPass skyRefractionPass{refractionBuffer};

	int width;
	int height;
};
