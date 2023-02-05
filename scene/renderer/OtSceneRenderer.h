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
#include "OtGbuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtSphereGeometry.h"
#include "OtUniformVec4.h"

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

	// get the gbuffer (for debugging purposes)
	void visualizeGbuffer();

private:
	// render passes
	// void renderShadowPass(OtScene2 scene);
	void renderGeometryPass(OtScene2 scene);
	void renderBackgroundPass(OtScene2 scene);
	void renderLightingPass(OtScene2 scene);

	// render entitities
	void renderSkyBox(OtSkyBoxComponent& component);
	void renderSkySphere(OtSkySphereComponent& component);
	void renderGeometry(OtScene2 scene, OtEntity entity);

	// target camera
	OtCamera2 camera;

	// various passes
	OtPass shadowPass;
	OtPass geometryPass;
	OtPass backgroundPass;
	OtPass lightingPass;

	// image dimensions
	int width;
	int height;

	// framebuffers
	OtGbuffer gbuffer;
	OtFrameBuffer composite{OtFrameBuffer::rgba8Texture, OtFrameBuffer::dFloatTexture, 1, true};

	// standard geometries
	OtBoxGeometry unityBoxGeometry;
	OtSphereGeometry unitySphereGeometry;

	// uniforms
	OtUniformVec4 materialUniforms{"u_material", 3};
	OtUniformVec4 backgroundUniforms{"u_background", 1};
	OtUniformVec4 lightingUniforms{"u_lighting", 3};

	// samplers
	OtSampler geometryAlbedoSampler{"s_geometryAlbedoTexture"};
	OtSampler geometryNormalSampler{"s_geometryNormalTexture"};
	OtSampler geometryMetallicSampler{"s_geometryMetallicTexture"};
	OtSampler geometryRoughnessSampler{"s_geometryRoughnessTexture"};
	OtSampler geometryAoSampler{"s_geometryAoTexture"};

	OtSampler skyMapSampler;
	OtSampler skySphereSampler;

	OtSampler lightingAlbedoSampler{"s_lightingAlbedoTexture"};
	OtSampler lightingPositionSampler{"s_lightingPositionTexture"};
	OtSampler lightingNormalSampler{"s_lightingNormalTexture"};
	OtSampler lightingPbrSampler{"s_lightingPbrTexture"};
	OtSampler lightingDepthSampler{"s_lightingDepthTexture"};

	// shaders
	OtShader geometryShader;
	OtShader skyBoxShader;
	OtShader skySphereShader;
	OtShader lightingShader;
};
