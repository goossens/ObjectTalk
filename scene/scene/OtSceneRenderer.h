//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtPass.h"

#include "OtCamera2.h"
#include "OtEntity.h"
#include "OtScene2.h"


//
//	OtSceneRenderer
//

class OtSceneRenderer {
public:
	// render the specified scene
	int render(OtScene2 scene, OtCamera2 camera, int width, int height);

private:
	// render passes
	// void renderShadowPass(OtScene2 scene);
	// void renderGeometryPass(OtScene2 scene);
	// void renderLightingPass();
	// void renderPostProcessingPass();
	void renderEnvironmentPass(OtScene2 scene);
	void renderCompositePass(OtScene2 scene);

	// target camera
	OtCamera2 camera;

	// various passes
	OtPass shadowPass;
	OtPass geometryPass;
	OtPass lightingPass;
	OtPass postProcessingPasses[4];
	OtPass environmentPass;
	OtPass compositePass;

	// graphical resources
	// CSM
	// gbuffer
	// light buffer
	// post process buffers
	int width;
	int height;

	OtFrameBuffer composite{OtFrameBuffer::rgba8Texture};
};
