//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComputeProgram.h"
#include "OtCubeMap.h"
#include "OtSampler.h"
#include "OtTexture.h"

#include "OtIblComponent.h"


//
//	OtImageBasedLighting
//

class OtImageBasedLighting {
public:
	// update (if required)
	void update(OtIblComponent& component);

	// textures
	OtTexture iblBrdfLut;

	// cubemaps
	OtCubeMap* iblSkyMap;
	OtCubeMap iblIrradianceMap;
	OtCubeMap iblEnvironmentMap;
	int maxEnvLevel;

private:
	// uniforms
	OtUniformVec4 iblEnviromentUniform{"u_iblEnvironment", 1};

	// samplers
	OtSampler cubemapSampler{"s_cubemap"};

	// compute programs
	OtComputeProgram brdfLutProgram{"OtBrdfLutCS"};
	OtComputeProgram irradianceProgram{"OtIblIrradianceMapCS"};
	OtComputeProgram envmapProgram{"OtIblEnvironmentMapCS"};

	// image based lighting data
	int iblSkyMapVersion = 0;
};
