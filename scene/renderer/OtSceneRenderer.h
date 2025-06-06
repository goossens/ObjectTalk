//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <memory>

#include "glm/glm.hpp"

#include "OtFrameBuffer.h"
#include "OtGbuffer.h"

#include "OtCamera.h"
#include "OtEntity.h"
#include "OtScene.h"

#include "OtImageBasedLighting.h"

#include "OtBackgroundPass.h"
#include "OtDeferredPass.h"
#include "OtForwardPass.h"
#include "OtOcclusionPass.h"
#include "OtParticlesPass.h"
#include "OtShadowPass.h"
#include "OtSkyPass.h"
#include "OtWaterPass.h"
#include "OtGridPass.h"
#include "OtHighlightPass.h"
#include "OtPickingPass.h"
#include "OtPostProcessingPass.h"


//
//	OtSceneRenderer
//

class OtSceneRenderer {
public:
	// set properties
	inline void setGridScale(float gridScale) { gridPass.setGridScale(gridScale); }
	inline void setSelectedEntity(OtEntity entity) { selectedEntity = entity; }

	// support entity picking
	inline void pickEntity(glm::vec2 ndc, std::function<void(OtEntity)> callback) {
		pickingNDC = ndc;
		pickingCallback = callback;
	}

	inline bool isPicking() { return pickingPass.isPicking(); }

	// render specified scene
	int render(OtCamera& camera, OtScene* scene);

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// framebuffers
	OtGbuffer deferredRenderingBuffer;
	OtFrameBuffer compositeBuffer{OtTexture::rgbaFloat16Texture, OtTexture::dFloatTexture};

	OtCascadedShadowMap csm;
	OtImageBasedLighting ibl;

	// rendering passes
	OtShadowPass shadowPass;
	OtBackgroundPass backgroundPass{compositeBuffer};
	OtSkyPass skyPass{compositeBuffer};
	OtDeferredPass deferredPass{deferredRenderingBuffer, compositeBuffer};
	OtForwardPass forwardPass{compositeBuffer};
	OtWaterPass waterPass{compositeBuffer};
	OtParticlesPass particlePass{compositeBuffer};
	OtGridPass gridPass{compositeBuffer};
	OtHighlightPass highlightPass{compositeBuffer};
	OtPickingPass pickingPass;
	OtPostProcessingPass postProcessingPass{compositeBuffer};

	// timings
	float iblPassTime;
	float shadowPassTime;
	float backgroundPassTime;
	float opaquePassTime;
	float transparentPassTime;
	float waterPassTime;
	float particlePassTime;
	float editorPassTime;
	float postProcessingTime;

	// support for selected entities and entity picking
	OtEntity selectedEntity = OtEntityNull;
	glm::vec2 pickingNDC;
	std::function<void(OtEntity)> pickingCallback;
};
