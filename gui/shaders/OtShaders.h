//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "OtBlendMappedShader.h"
#include "OtBlitShader.h"
#include "OtBloomShader.h"
#include "OtBlurShader.h"
#include "OtColoredShader.h"
#include "OtDebugShader.h"
#include "OtFixedShader.h"
#include "OtGeometryShader.h"
#include "OtImGuiShader.h"
#include "OtLightingShader.h"
#include "OtShadowShader.h"
#include "OtSkyShader.h"
#include "OtSkyboxShader.h"
#include "OtSkySphereShader.h"
#include "OtTerrainShader.h"
#include "OtTexturedShader.h"
#include "OtTransparentShadowShader.h"
#include "OtVectorDisplayShader.h"
#include "OtWaterShader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBlendMappedVS),
	BGFX_EMBEDDED_SHADER(OtBlendMappedFS),
	BGFX_EMBEDDED_SHADER(OtBlendMappedVSI),
	BGFX_EMBEDDED_SHADER(OtBlitVS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBloomVS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBlurVS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtColoredVS),
	BGFX_EMBEDDED_SHADER(OtColoredFS),
	BGFX_EMBEDDED_SHADER(OtColoredVSI),
	BGFX_EMBEDDED_SHADER(OtDebugVS),
	BGFX_EMBEDDED_SHADER(OtDebugFS),
	BGFX_EMBEDDED_SHADER(OtFixedVS),
	BGFX_EMBEDDED_SHADER(OtFixedFS),
	BGFX_EMBEDDED_SHADER(OtFixedVSI),
	BGFX_EMBEDDED_SHADER(OtGeometryVS),
	BGFX_EMBEDDED_SHADER(OtGeometryFS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtLightingVS),
	BGFX_EMBEDDED_SHADER(OtLightingFS),
	BGFX_EMBEDDED_SHADER(OtShadowVS),
	BGFX_EMBEDDED_SHADER(OtShadowFS),
	BGFX_EMBEDDED_SHADER(OtShadowVSI),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkyboxVS),
	BGFX_EMBEDDED_SHADER(OtSkyboxFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereVS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtTexturedVS),
	BGFX_EMBEDDED_SHADER(OtTexturedFS),
	BGFX_EMBEDDED_SHADER(OtTexturedVSI),
	BGFX_EMBEDDED_SHADER(OtTransparentShadowVS),
	BGFX_EMBEDDED_SHADER(OtTransparentShadowFS),
	BGFX_EMBEDDED_SHADER(OtTransparentShadowVSI),
	BGFX_EMBEDDED_SHADER(OtVectorDisplayVS),
	BGFX_EMBEDDED_SHADER(OtVectorDisplayFS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),
	BGFX_EMBEDDED_SHADER_END()
};
