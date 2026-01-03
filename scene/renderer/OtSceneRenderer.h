//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtCamera.h"
#include "OtCascadedShadowMap.h"
#include "OtFrameBuffer.h"
#include "OtGbuffer.h"

#include "OtEntity.h"
#include "OtScene.h"

#include "OtImageBasedLighting.h"

#include "OtBackgroundPass.h"
#include "OtDeferredPass.h"
#include "OtForwardPass.h"
#include "OtParticlesPass.h"
#include "OtShadowPass.h"
#include "OtSkyPass.h"
#include "OtWaterPass.h"
#include "OtGridPass.h"
#include "OtHighlightPass.h"
#include "OtPickingPass.h"
#include "OtPostProcessingPass.h"
#include "OtSceneRendererContext.h"


//
//	OtSceneRenderer
//

class OtSceneRenderer {
public:
	// set properties
	inline void setGridScale(float gridScale) { gridPass.setGridScale(gridScale); }
	inline void setSelectedEntity(OtEntity entity) { selectedEntity = entity; }

	// support entity picking
	inline void pickEntity(glm::vec2 uv, std::function<void(OtEntity)> callback) {
		pickingUV = uv;
		pickingCallback = callback;
	}

	// render specified scene
	ImTextureID render(OtCamera& camera, OtScene* scene);

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// rendering context
	OtSceneRendererContext ctx;

	// framebuffers
	OtGbuffer deferredRenderingBuffer;
	OtFrameBuffer compositeBuffer{OtTexture::Format::rgba16, OtTexture::Format::d32};

	// rendering passes
	OtShadowPass shadowPass;
	OtBackgroundPass backgroundPass{compositeBuffer};
	OtDeferredPass deferredPass{deferredRenderingBuffer, compositeBuffer};
	OtForwardPass forwardPass{compositeBuffer};
	OtSkyPass skyPass{compositeBuffer};
	OtWaterPass waterPass{compositeBuffer};
	OtParticlesPass particlePass{compositeBuffer};
	OtGridPass gridPass{compositeBuffer};
	OtPostProcessingPass postProcessingPass{compositeBuffer};
	OtHighlightPass highlightPass;
	OtPickingPass pickingPass;

	// timings
	float ctxTime;
	float shadowPassTime;
	float backgroundPassTime;
	float opaquePassTime;
	float waterPassTime;
	float skyPassTime;
	float particlePassTime;
	float transparentPassTime;
	float gridPassTime;
	float postProcessingTime;
	float editorPassTime;
	float renderTime;

	// support for selected entities and entity picking
	OtEntity selectedEntity = OtEntityNull;
	glm::vec2 pickingUV;
	std::function<void(OtEntity)> pickingCallback;
};
