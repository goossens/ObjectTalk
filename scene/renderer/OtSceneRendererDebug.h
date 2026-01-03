//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <cstdint>

#include "imgui.h"

#include "OtCubeMap.h"
#include "OtFrameBuffer.h"
#include "OtIndexBuffer.h"
#include "OtRenderPipeline.h"
#include "OtTexture.h"
#include "OtVertexBuffer.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRendererDebug
//

class OtSceneRendererDebug {
public:
	// render the debug window
	void render(OtSceneRenderer& renderer);

private:
	struct CubeMapDebug {
		OtFrameBuffer framebuffer{OtTexture::Format::rgba8};
		int renderedVersion = 0;
		int renderedMip = 0;
		int requestedMip = 0;
	};

	// internal rendering functions
	void renderIbl(OtSceneRenderer& renderer);
	void renderGbuffer(OtSceneRenderer& renderer);
	void renderShadowMaps(OtSceneRenderer& renderer);
	void renderReflection(OtSceneRenderer& renderer);
	void renderOcclusion(OtSceneRenderer& renderer);
	void renderTimings(OtSceneRenderer& renderer);
	void renderAssets();

	void renderTexture(const char* title, ImTextureID index, int width, int height);
	void renderTexture(const char* title, OtTexture& texture);
	void renderCubeMap(const char* title, OtCubeMap& cubemap, CubeMapDebug& debug);
	void renderCubeMapAsCross(OtCubeMap& cubemap, CubeMapDebug& debug);

	// properties
	CubeMapDebug iblSkyMapDebug;
	CubeMapDebug iblIrradianceDebug;
	CubeMapDebug iblEnvironmentDebug;

	OtRenderPipeline pipeline;
	OtIndexBuffer indexBuffer;
	OtVertexBuffer vertexBuffer;

	void initializeResources();
	bool resourcesInitialized = false;
};
