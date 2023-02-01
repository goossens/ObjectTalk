//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBoxGeometry.h"
#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtSphereGeometry.h"

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

	// render entitities
	void renderSkyBox(OtSkyBoxComponent& component);
	void renderSkySphere(OtSkySphereComponent& component);

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

	// image dimensions
	int width;
	int height;

	// framebuffers
	OtFrameBuffer composite{OtFrameBuffer::rgba8Texture};

	// standard geometries
	OtBoxGeometry unityBoxGeometry;
	OtSphereGeometry unitySphereGeometry;

	// samplers
	OtSampler skyMapSampler;
	OtSampler skySphereSampler;

	OtSampler albedoSampler;
	OtSampler normalSampler;
	OtSampler metalicSampler;
	OtSampler roughnessSampler;
	OtSampler aoSampler;

	// shaders
	OtShader skyBoxShader;
	OtShader skySphereShader;

};
