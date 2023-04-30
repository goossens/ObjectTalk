//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "generated/OtBloomDownSampleShader.h"
#include "generated/OtBloomUpSampleShader.h"
#include "generated/OtDebugShader.h"
#include "generated/OtGeometryShader.h"
#include "generated/OtGridShader.h"
#include "generated/OtImGuiShader.h"
#include "generated/OtLightingShader.h"
#include "generated/OtOutlineShader.h"
#include "generated/OtPostProcessShader.h"
#include "generated/OtSelectShader.h"
#include "generated/OtSkyShader.h"
#include "generated/OtSkySphereShader.h"
#include "generated/OtSkyboxShader.h"
#include "generated/OtTerrainShader.h"
#include "generated/OtTransparentShader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleVS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleVS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtDebugVS),
	BGFX_EMBEDDED_SHADER(OtDebugFS),
	BGFX_EMBEDDED_SHADER(OtGeometryVS),
	BGFX_EMBEDDED_SHADER(OtGeometryFS),
	BGFX_EMBEDDED_SHADER(OtGeometryVSI),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtLightingVS),
	BGFX_EMBEDDED_SHADER(OtLightingFS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessVS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereVS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtSkyboxVS),
	BGFX_EMBEDDED_SHADER(OtSkyboxFS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtTransparentVS),
	BGFX_EMBEDDED_SHADER(OtTransparentFS),
	BGFX_EMBEDDED_SHADER(OtTransparentVSI),
	BGFX_EMBEDDED_SHADER_END()
};
