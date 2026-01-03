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
#include "OtGbuffer.h"
#include "OtIndexBuffer.h"
#include "OtRenderPass.h"
#include "OtRenderPipeline.h"
#include "OtVertexBuffer.h"

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
	void renderReflection(OtSceneRendererContext& ctx, OtWaterComponent& water);
	void renderRefraction(OtSceneRendererContext& ctx, OtWaterComponent& water);
	void renderWater(OtSceneRendererContext& ctx, OtWaterComponent& water);

	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// properties
	OtFrameBuffer& framebuffer;

	OtGbuffer renderingBuffer;
	OtFrameBuffer reflectionBuffer{OtTexture::Format::rgba16, OtTexture::Format::d32};
	OtFrameBuffer refractionBuffer{OtTexture::Format::rgba16, OtTexture::Format::d32};

	OtIndexBuffer indexBuffer;
	OtVertexBuffer vertexBuffer;

	OtRenderPipeline waterPipeline;

	OtBackgroundPass backgroundReflectionPass{reflectionBuffer};
	OtDeferredPass deferredReflectionPass{renderingBuffer, reflectionBuffer};
	OtForwardPass forwardReflectionPass{reflectionBuffer};
	OtSkyPass skyReflectionPass{reflectionBuffer};

	OtBackgroundPass backgroundRefractionPass{refractionBuffer};
	OtDeferredPass deferredRefractionPass{renderingBuffer, refractionBuffer};
	OtForwardPass forwardRefractionPass{refractionBuffer};

	int width;
	int height;

	// support functions
	void initializeResources();
	bool resourcesInitialized = false;
};
