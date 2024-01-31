//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "generated/deferred/OtDeferredInstancingVS_mtl.h"
#include "generated/deferred/OtDeferredInstancingVS_glsl.h"
#include "generated/deferred/OtDeferredInstancingVS_spv.h"
#include "generated/deferred/OtDeferredInstancingVS_essl.h"
#include "generated/deferred/OtDeferredInstancingVS_dx11.h"
#include "generated/deferred/OtDeferredLightingVS_mtl.h"
#include "generated/deferred/OtDeferredLightingVS_glsl.h"
#include "generated/deferred/OtDeferredLightingVS_spv.h"
#include "generated/deferred/OtDeferredLightingVS_essl.h"
#include "generated/deferred/OtDeferredLightingVS_dx11.h"
#include "generated/deferred/OtDeferredVS_mtl.h"
#include "generated/deferred/OtDeferredVS_glsl.h"
#include "generated/deferred/OtDeferredVS_spv.h"
#include "generated/deferred/OtDeferredVS_essl.h"
#include "generated/deferred/OtDeferredVS_dx11.h"
#include "generated/deferred/OtDeferredLightingFS_mtl.h"
#include "generated/deferred/OtDeferredLightingFS_glsl.h"
#include "generated/deferred/OtDeferredLightingFS_spv.h"
#include "generated/deferred/OtDeferredLightingFS_essl.h"
#include "generated/deferred/OtDeferredLightingFS_dx11.h"
#include "generated/deferred/OtDeferredPbrFS_mtl.h"
#include "generated/deferred/OtDeferredPbrFS_glsl.h"
#include "generated/deferred/OtDeferredPbrFS_spv.h"
#include "generated/deferred/OtDeferredPbrFS_essl.h"
#include "generated/deferred/OtDeferredPbrFS_dx11.h"
#include "generated/filter/OtFilterVS_mtl.h"
#include "generated/filter/OtFilterVS_glsl.h"
#include "generated/filter/OtFilterVS_spv.h"
#include "generated/filter/OtFilterVS_essl.h"
#include "generated/filter/OtFilterVS_dx11.h"
#include "generated/filter/OtBlitFS_mtl.h"
#include "generated/filter/OtBlitFS_glsl.h"
#include "generated/filter/OtBlitFS_spv.h"
#include "generated/filter/OtBlitFS_essl.h"
#include "generated/filter/OtBlitFS_dx11.h"
#include "generated/filter/OtBloomDownSampleFS_mtl.h"
#include "generated/filter/OtBloomDownSampleFS_glsl.h"
#include "generated/filter/OtBloomDownSampleFS_spv.h"
#include "generated/filter/OtBloomDownSampleFS_essl.h"
#include "generated/filter/OtBloomDownSampleFS_dx11.h"
#include "generated/filter/OtBloomFS_mtl.h"
#include "generated/filter/OtBloomFS_glsl.h"
#include "generated/filter/OtBloomFS_spv.h"
#include "generated/filter/OtBloomFS_essl.h"
#include "generated/filter/OtBloomFS_dx11.h"
#include "generated/filter/OtBloomUpSampleFS_mtl.h"
#include "generated/filter/OtBloomUpSampleFS_glsl.h"
#include "generated/filter/OtBloomUpSampleFS_spv.h"
#include "generated/filter/OtBloomUpSampleFS_essl.h"
#include "generated/filter/OtBloomUpSampleFS_dx11.h"
#include "generated/filter/OtBlurFS_mtl.h"
#include "generated/filter/OtBlurFS_glsl.h"
#include "generated/filter/OtBlurFS_spv.h"
#include "generated/filter/OtBlurFS_essl.h"
#include "generated/filter/OtBlurFS_dx11.h"
#include "generated/filter/OtIslandizerFS_mtl.h"
#include "generated/filter/OtIslandizerFS_glsl.h"
#include "generated/filter/OtIslandizerFS_spv.h"
#include "generated/filter/OtIslandizerFS_essl.h"
#include "generated/filter/OtIslandizerFS_dx11.h"
#include "generated/filter/OtNormalMapperFS_mtl.h"
#include "generated/filter/OtNormalMapperFS_glsl.h"
#include "generated/filter/OtNormalMapperFS_spv.h"
#include "generated/filter/OtNormalMapperFS_essl.h"
#include "generated/filter/OtNormalMapperFS_dx11.h"
#include "generated/filter/OtPostProcessFS_mtl.h"
#include "generated/filter/OtPostProcessFS_glsl.h"
#include "generated/filter/OtPostProcessFS_spv.h"
#include "generated/filter/OtPostProcessFS_essl.h"
#include "generated/filter/OtPostProcessFS_dx11.h"
#include "generated/filter/OtSeamlessTileFS_mtl.h"
#include "generated/filter/OtSeamlessTileFS_glsl.h"
#include "generated/filter/OtSeamlessTileFS_spv.h"
#include "generated/filter/OtSeamlessTileFS_essl.h"
#include "generated/filter/OtSeamlessTileFS_dx11.h"
#include "generated/forward/OtForwardInstancingVS_mtl.h"
#include "generated/forward/OtForwardInstancingVS_glsl.h"
#include "generated/forward/OtForwardInstancingVS_spv.h"
#include "generated/forward/OtForwardInstancingVS_essl.h"
#include "generated/forward/OtForwardInstancingVS_dx11.h"
#include "generated/forward/OtForwardVS_mtl.h"
#include "generated/forward/OtForwardVS_glsl.h"
#include "generated/forward/OtForwardVS_spv.h"
#include "generated/forward/OtForwardVS_essl.h"
#include "generated/forward/OtForwardVS_dx11.h"
#include "generated/forward/OtForwardPbrFS_mtl.h"
#include "generated/forward/OtForwardPbrFS_glsl.h"
#include "generated/forward/OtForwardPbrFS_spv.h"
#include "generated/forward/OtForwardPbrFS_essl.h"
#include "generated/forward/OtForwardPbrFS_dx11.h"
#include "generated/generator/OtGeneratorVS_mtl.h"
#include "generated/generator/OtGeneratorVS_glsl.h"
#include "generated/generator/OtGeneratorVS_spv.h"
#include "generated/generator/OtGeneratorVS_essl.h"
#include "generated/generator/OtGeneratorVS_dx11.h"
#include "generated/generator/OtFbmFS_mtl.h"
#include "generated/generator/OtFbmFS_glsl.h"
#include "generated/generator/OtFbmFS_spv.h"
#include "generated/generator/OtFbmFS_essl.h"
#include "generated/generator/OtFbmFS_dx11.h"
#include "generated/generator/OtTileableFbmFS_mtl.h"
#include "generated/generator/OtTileableFbmFS_glsl.h"
#include "generated/generator/OtTileableFbmFS_spv.h"
#include "generated/generator/OtTileableFbmFS_essl.h"
#include "generated/generator/OtTileableFbmFS_dx11.h"
#include "generated/grid/OtGridVS_mtl.h"
#include "generated/grid/OtGridVS_glsl.h"
#include "generated/grid/OtGridVS_spv.h"
#include "generated/grid/OtGridVS_essl.h"
#include "generated/grid/OtGridVS_dx11.h"
#include "generated/grid/OtGridFS_mtl.h"
#include "generated/grid/OtGridFS_glsl.h"
#include "generated/grid/OtGridFS_spv.h"
#include "generated/grid/OtGridFS_essl.h"
#include "generated/grid/OtGridFS_dx11.h"
#include "generated/highlight/OtOutlineVS_mtl.h"
#include "generated/highlight/OtOutlineVS_glsl.h"
#include "generated/highlight/OtOutlineVS_spv.h"
#include "generated/highlight/OtOutlineVS_essl.h"
#include "generated/highlight/OtOutlineVS_dx11.h"
#include "generated/highlight/OtSelectVS_mtl.h"
#include "generated/highlight/OtSelectVS_glsl.h"
#include "generated/highlight/OtSelectVS_spv.h"
#include "generated/highlight/OtSelectVS_essl.h"
#include "generated/highlight/OtSelectVS_dx11.h"
#include "generated/highlight/OtOutlineFS_mtl.h"
#include "generated/highlight/OtOutlineFS_glsl.h"
#include "generated/highlight/OtOutlineFS_spv.h"
#include "generated/highlight/OtOutlineFS_essl.h"
#include "generated/highlight/OtOutlineFS_dx11.h"
#include "generated/highlight/OtSelectFS_mtl.h"
#include "generated/highlight/OtSelectFS_glsl.h"
#include "generated/highlight/OtSelectFS_spv.h"
#include "generated/highlight/OtSelectFS_essl.h"
#include "generated/highlight/OtSelectFS_dx11.h"
#include "generated/imgui/OtImGuiVS_mtl.h"
#include "generated/imgui/OtImGuiVS_glsl.h"
#include "generated/imgui/OtImGuiVS_spv.h"
#include "generated/imgui/OtImGuiVS_essl.h"
#include "generated/imgui/OtImGuiVS_dx11.h"
#include "generated/imgui/OtImGuiFS_mtl.h"
#include "generated/imgui/OtImGuiFS_glsl.h"
#include "generated/imgui/OtImGuiFS_spv.h"
#include "generated/imgui/OtImGuiFS_essl.h"
#include "generated/imgui/OtImGuiFS_dx11.h"
#include "generated/sky/OtSkyVS_mtl.h"
#include "generated/sky/OtSkyVS_glsl.h"
#include "generated/sky/OtSkyVS_spv.h"
#include "generated/sky/OtSkyVS_essl.h"
#include "generated/sky/OtSkyVS_dx11.h"
#include "generated/sky/OtSkyBoxFS_mtl.h"
#include "generated/sky/OtSkyBoxFS_glsl.h"
#include "generated/sky/OtSkyBoxFS_spv.h"
#include "generated/sky/OtSkyBoxFS_essl.h"
#include "generated/sky/OtSkyBoxFS_dx11.h"
#include "generated/sky/OtSkyFS_mtl.h"
#include "generated/sky/OtSkyFS_glsl.h"
#include "generated/sky/OtSkyFS_spv.h"
#include "generated/sky/OtSkyFS_essl.h"
#include "generated/sky/OtSkyFS_dx11.h"
#include "generated/sky/OtSkySphereFS_mtl.h"
#include "generated/sky/OtSkySphereFS_glsl.h"
#include "generated/sky/OtSkySphereFS_spv.h"
#include "generated/sky/OtSkySphereFS_essl.h"
#include "generated/sky/OtSkySphereFS_dx11.h"
#include "generated/terrain/OtTerrainVS_mtl.h"
#include "generated/terrain/OtTerrainVS_glsl.h"
#include "generated/terrain/OtTerrainVS_spv.h"
#include "generated/terrain/OtTerrainVS_essl.h"
#include "generated/terrain/OtTerrainVS_dx11.h"
#include "generated/terrain/OtTerrainFS_mtl.h"
#include "generated/terrain/OtTerrainFS_glsl.h"
#include "generated/terrain/OtTerrainFS_spv.h"
#include "generated/terrain/OtTerrainFS_essl.h"
#include "generated/terrain/OtTerrainFS_dx11.h"
#include "generated/water/OtWaterVS_mtl.h"
#include "generated/water/OtWaterVS_glsl.h"
#include "generated/water/OtWaterVS_spv.h"
#include "generated/water/OtWaterVS_essl.h"
#include "generated/water/OtWaterVS_dx11.h"
#include "generated/water/OtWaterFS_mtl.h"
#include "generated/water/OtWaterFS_glsl.h"
#include "generated/water/OtWaterFS_spv.h"
#include "generated/water/OtWaterFS_essl.h"
#include "generated/water/OtWaterFS_dx11.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtGeneratorVS),
	BGFX_EMBEDDED_SHADER(OtGeneratorVS),
	BGFX_EMBEDDED_SHADER(OtGeneratorVS),
	BGFX_EMBEDDED_SHADER(OtGeneratorVS),
	BGFX_EMBEDDED_SHADER(OtGeneratorVS),
	BGFX_EMBEDDED_SHADER(OtFbmFS),
	BGFX_EMBEDDED_SHADER(OtFbmFS),
	BGFX_EMBEDDED_SHADER(OtFbmFS),
	BGFX_EMBEDDED_SHADER(OtFbmFS),
	BGFX_EMBEDDED_SHADER(OtFbmFS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmFS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmFS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmFS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmFS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmFS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtSkySphereFS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),
	BGFX_EMBEDDED_SHADER_END()
};
