//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCubeMap.h"
#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"
#include "OtUniformVec4.h"

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
		OtFrameBuffer framebuffer{OtTexture::rgba8Texture};
		int renderedVersion = 0;
		int renderedMip = 0;
		int requestedMip = 0;
	};

	// internal rendering functions
	void renderIbl(OtSceneRenderer& renderer);
	void renderReflection(OtSceneRenderer& renderer);

	void renderTexture(const char* title, OtTexture& texture);
	void renderCubeMap(const char* title, OtCubeMap& cubemap, CubeMapDebug& debug);
	void renderCubeMapAsCross(OtCubeMap& cubemap, CubeMapDebug& debug);

	// properties to help debug rendering
	CubeMapDebug iblSkyMapDebug;
	CubeMapDebug iblIrradianceDebug;
	CubeMapDebug iblEnvironmentDebug;

	OtSampler crossSampler{"s_cubemap"};
	OtUniformVec4 crossUniform{"u_crossUniform", 1};
	OtShaderProgram crossShader{"OtCubeMapCrossVS", "OtCubeMapCrossFS"};
};