//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "OtFormat.h"
#include "OtLog.h"

#include "OtShaders.h"
#include "generated/cubemap/OtCubeMapCrossVS_mtl.h"
#include "generated/cubemap/OtCubeMapCrossVS_spv.h"
#include "generated/cubemap/OtCubeMapCrossVS_dx11.h"
#include "generated/cubemap/OtHdrReprojectVS_mtl.h"
#include "generated/cubemap/OtHdrReprojectVS_spv.h"
#include "generated/cubemap/OtHdrReprojectVS_dx11.h"
#include "generated/cubemap/OtCubeMapCrossFS_mtl.h"
#include "generated/cubemap/OtCubeMapCrossFS_spv.h"
#include "generated/cubemap/OtCubeMapCrossFS_dx11.h"
#include "generated/cubemap/OtHdrReprojectFS_mtl.h"
#include "generated/cubemap/OtHdrReprojectFS_spv.h"
#include "generated/cubemap/OtHdrReprojectFS_dx11.h"
#include "generated/deferred/OtDeferredInstancingVS_mtl.h"
#include "generated/deferred/OtDeferredInstancingVS_spv.h"
#include "generated/deferred/OtDeferredInstancingVS_dx11.h"
#include "generated/deferred/OtDeferredLightingVS_mtl.h"
#include "generated/deferred/OtDeferredLightingVS_spv.h"
#include "generated/deferred/OtDeferredLightingVS_dx11.h"
#include "generated/deferred/OtDeferredVS_mtl.h"
#include "generated/deferred/OtDeferredVS_spv.h"
#include "generated/deferred/OtDeferredVS_dx11.h"
#include "generated/deferred/OtDeferredLightingFS_mtl.h"
#include "generated/deferred/OtDeferredLightingFS_spv.h"
#include "generated/deferred/OtDeferredLightingFS_dx11.h"
#include "generated/deferred/OtDeferredPbrFS_mtl.h"
#include "generated/deferred/OtDeferredPbrFS_spv.h"
#include "generated/deferred/OtDeferredPbrFS_dx11.h"
#include "generated/filter/OtFilterVS_mtl.h"
#include "generated/filter/OtFilterVS_spv.h"
#include "generated/filter/OtFilterVS_dx11.h"
#include "generated/filter/OtBlitFS_mtl.h"
#include "generated/filter/OtBlitFS_spv.h"
#include "generated/filter/OtBlitFS_dx11.h"
#include "generated/filter/OtBloomDownSampleFS_mtl.h"
#include "generated/filter/OtBloomDownSampleFS_spv.h"
#include "generated/filter/OtBloomDownSampleFS_dx11.h"
#include "generated/filter/OtBloomFS_mtl.h"
#include "generated/filter/OtBloomFS_spv.h"
#include "generated/filter/OtBloomFS_dx11.h"
#include "generated/filter/OtBloomUpSampleFS_mtl.h"
#include "generated/filter/OtBloomUpSampleFS_spv.h"
#include "generated/filter/OtBloomUpSampleFS_dx11.h"
#include "generated/filter/OtBlurFS_mtl.h"
#include "generated/filter/OtBlurFS_spv.h"
#include "generated/filter/OtBlurFS_dx11.h"
#include "generated/filter/OtIslandizerFS_mtl.h"
#include "generated/filter/OtIslandizerFS_spv.h"
#include "generated/filter/OtIslandizerFS_dx11.h"
#include "generated/filter/OtNormalMapperFS_mtl.h"
#include "generated/filter/OtNormalMapperFS_spv.h"
#include "generated/filter/OtNormalMapperFS_dx11.h"
#include "generated/filter/OtPostProcessFS_mtl.h"
#include "generated/filter/OtPostProcessFS_spv.h"
#include "generated/filter/OtPostProcessFS_dx11.h"
#include "generated/filter/OtSeamlessTileFS_mtl.h"
#include "generated/filter/OtSeamlessTileFS_spv.h"
#include "generated/filter/OtSeamlessTileFS_dx11.h"
#include "generated/forward/OtForwardInstancingVS_mtl.h"
#include "generated/forward/OtForwardInstancingVS_spv.h"
#include "generated/forward/OtForwardInstancingVS_dx11.h"
#include "generated/forward/OtForwardVS_mtl.h"
#include "generated/forward/OtForwardVS_spv.h"
#include "generated/forward/OtForwardVS_dx11.h"
#include "generated/forward/OtForwardPbrFS_mtl.h"
#include "generated/forward/OtForwardPbrFS_spv.h"
#include "generated/forward/OtForwardPbrFS_dx11.h"
#include "generated/generator/OtGeneratorVS_mtl.h"
#include "generated/generator/OtGeneratorVS_spv.h"
#include "generated/generator/OtGeneratorVS_dx11.h"
#include "generated/generator/OtFbmFS_mtl.h"
#include "generated/generator/OtFbmFS_spv.h"
#include "generated/generator/OtFbmFS_dx11.h"
#include "generated/generator/OtTileableFbmFS_mtl.h"
#include "generated/generator/OtTileableFbmFS_spv.h"
#include "generated/generator/OtTileableFbmFS_dx11.h"
#include "generated/grid/OtGridVS_mtl.h"
#include "generated/grid/OtGridVS_spv.h"
#include "generated/grid/OtGridVS_dx11.h"
#include "generated/grid/OtGridFS_mtl.h"
#include "generated/grid/OtGridFS_spv.h"
#include "generated/grid/OtGridFS_dx11.h"
#include "generated/highlight/OtOutlineVS_mtl.h"
#include "generated/highlight/OtOutlineVS_spv.h"
#include "generated/highlight/OtOutlineVS_dx11.h"
#include "generated/highlight/OtSelectVS_mtl.h"
#include "generated/highlight/OtSelectVS_spv.h"
#include "generated/highlight/OtSelectVS_dx11.h"
#include "generated/highlight/OtOutlineFS_mtl.h"
#include "generated/highlight/OtOutlineFS_spv.h"
#include "generated/highlight/OtOutlineFS_dx11.h"
#include "generated/highlight/OtSelectFS_mtl.h"
#include "generated/highlight/OtSelectFS_spv.h"
#include "generated/highlight/OtSelectFS_dx11.h"
#include "generated/ibl/OtBrdfLutCS_mtl.h"
#include "generated/ibl/OtBrdfLutCS_spv.h"
#include "generated/ibl/OtBrdfLutCS_dx11.h"
#include "generated/ibl/OtIblEnvironmentMapCS_mtl.h"
#include "generated/ibl/OtIblEnvironmentMapCS_spv.h"
#include "generated/ibl/OtIblEnvironmentMapCS_dx11.h"
#include "generated/ibl/OtIblIrradianceMapCS_mtl.h"
#include "generated/ibl/OtIblIrradianceMapCS_spv.h"
#include "generated/ibl/OtIblIrradianceMapCS_dx11.h"
#include "generated/imgui/OtImGuiVS_mtl.h"
#include "generated/imgui/OtImGuiVS_spv.h"
#include "generated/imgui/OtImGuiVS_dx11.h"
#include "generated/imgui/OtImGuiFS_mtl.h"
#include "generated/imgui/OtImGuiFS_spv.h"
#include "generated/imgui/OtImGuiFS_dx11.h"
#include "generated/sky/OtSkyVS_mtl.h"
#include "generated/sky/OtSkyVS_spv.h"
#include "generated/sky/OtSkyVS_dx11.h"
#include "generated/sky/OtSkyBoxFS_mtl.h"
#include "generated/sky/OtSkyBoxFS_spv.h"
#include "generated/sky/OtSkyBoxFS_dx11.h"
#include "generated/sky/OtSkyFS_mtl.h"
#include "generated/sky/OtSkyFS_spv.h"
#include "generated/sky/OtSkyFS_dx11.h"
#include "generated/terrain/OtTerrainVS_mtl.h"
#include "generated/terrain/OtTerrainVS_spv.h"
#include "generated/terrain/OtTerrainVS_dx11.h"
#include "generated/terrain/OtTerrainFS_mtl.h"
#include "generated/terrain/OtTerrainFS_spv.h"
#include "generated/terrain/OtTerrainFS_dx11.h"
#include "generated/water/OtWaterVS_mtl.h"
#include "generated/water/OtWaterVS_spv.h"
#include "generated/water/OtWaterVS_dx11.h"
#include "generated/water/OtWaterFS_mtl.h"
#include "generated/water/OtWaterFS_spv.h"
#include "generated/water/OtWaterFS_dx11.h"



//
//	Globals
//

static const uint8_t OtCubeMapCrossVS_glsl[1] = {0};
static const uint8_t OtCubeMapCrossVS_essl[1] = {0};
static const uint8_t OtHdrReprojectVS_glsl[1] = {0};
static const uint8_t OtHdrReprojectVS_essl[1] = {0};
static const uint8_t OtCubeMapCrossFS_glsl[1] = {0};
static const uint8_t OtCubeMapCrossFS_essl[1] = {0};
static const uint8_t OtHdrReprojectFS_glsl[1] = {0};
static const uint8_t OtHdrReprojectFS_essl[1] = {0};
static const uint8_t OtDeferredInstancingVS_glsl[1] = {0};
static const uint8_t OtDeferredInstancingVS_essl[1] = {0};
static const uint8_t OtDeferredLightingVS_glsl[1] = {0};
static const uint8_t OtDeferredLightingVS_essl[1] = {0};
static const uint8_t OtDeferredVS_glsl[1] = {0};
static const uint8_t OtDeferredVS_essl[1] = {0};
static const uint8_t OtDeferredLightingFS_glsl[1] = {0};
static const uint8_t OtDeferredLightingFS_essl[1] = {0};
static const uint8_t OtDeferredPbrFS_glsl[1] = {0};
static const uint8_t OtDeferredPbrFS_essl[1] = {0};
static const uint8_t OtFilterVS_glsl[1] = {0};
static const uint8_t OtFilterVS_essl[1] = {0};
static const uint8_t OtBlitFS_glsl[1] = {0};
static const uint8_t OtBlitFS_essl[1] = {0};
static const uint8_t OtBloomDownSampleFS_glsl[1] = {0};
static const uint8_t OtBloomDownSampleFS_essl[1] = {0};
static const uint8_t OtBloomFS_glsl[1] = {0};
static const uint8_t OtBloomFS_essl[1] = {0};
static const uint8_t OtBloomUpSampleFS_glsl[1] = {0};
static const uint8_t OtBloomUpSampleFS_essl[1] = {0};
static const uint8_t OtBlurFS_glsl[1] = {0};
static const uint8_t OtBlurFS_essl[1] = {0};
static const uint8_t OtIslandizerFS_glsl[1] = {0};
static const uint8_t OtIslandizerFS_essl[1] = {0};
static const uint8_t OtNormalMapperFS_glsl[1] = {0};
static const uint8_t OtNormalMapperFS_essl[1] = {0};
static const uint8_t OtPostProcessFS_glsl[1] = {0};
static const uint8_t OtPostProcessFS_essl[1] = {0};
static const uint8_t OtSeamlessTileFS_glsl[1] = {0};
static const uint8_t OtSeamlessTileFS_essl[1] = {0};
static const uint8_t OtForwardInstancingVS_glsl[1] = {0};
static const uint8_t OtForwardInstancingVS_essl[1] = {0};
static const uint8_t OtForwardVS_glsl[1] = {0};
static const uint8_t OtForwardVS_essl[1] = {0};
static const uint8_t OtForwardPbrFS_glsl[1] = {0};
static const uint8_t OtForwardPbrFS_essl[1] = {0};
static const uint8_t OtGeneratorVS_glsl[1] = {0};
static const uint8_t OtGeneratorVS_essl[1] = {0};
static const uint8_t OtFbmFS_glsl[1] = {0};
static const uint8_t OtFbmFS_essl[1] = {0};
static const uint8_t OtTileableFbmFS_glsl[1] = {0};
static const uint8_t OtTileableFbmFS_essl[1] = {0};
static const uint8_t OtGridVS_glsl[1] = {0};
static const uint8_t OtGridVS_essl[1] = {0};
static const uint8_t OtGridFS_glsl[1] = {0};
static const uint8_t OtGridFS_essl[1] = {0};
static const uint8_t OtOutlineVS_glsl[1] = {0};
static const uint8_t OtOutlineVS_essl[1] = {0};
static const uint8_t OtSelectVS_glsl[1] = {0};
static const uint8_t OtSelectVS_essl[1] = {0};
static const uint8_t OtOutlineFS_glsl[1] = {0};
static const uint8_t OtOutlineFS_essl[1] = {0};
static const uint8_t OtSelectFS_glsl[1] = {0};
static const uint8_t OtSelectFS_essl[1] = {0};
static const uint8_t OtBrdfLutCS_glsl[1] = {0};
static const uint8_t OtBrdfLutCS_essl[1] = {0};
static const uint8_t OtIblEnvironmentMapCS_glsl[1] = {0};
static const uint8_t OtIblEnvironmentMapCS_essl[1] = {0};
static const uint8_t OtIblIrradianceMapCS_glsl[1] = {0};
static const uint8_t OtIblIrradianceMapCS_essl[1] = {0};
static const uint8_t OtImGuiVS_glsl[1] = {0};
static const uint8_t OtImGuiVS_essl[1] = {0};
static const uint8_t OtImGuiFS_glsl[1] = {0};
static const uint8_t OtImGuiFS_essl[1] = {0};
static const uint8_t OtSkyVS_glsl[1] = {0};
static const uint8_t OtSkyVS_essl[1] = {0};
static const uint8_t OtSkyBoxFS_glsl[1] = {0};
static const uint8_t OtSkyBoxFS_essl[1] = {0};
static const uint8_t OtSkyFS_glsl[1] = {0};
static const uint8_t OtSkyFS_essl[1] = {0};
static const uint8_t OtTerrainVS_glsl[1] = {0};
static const uint8_t OtTerrainVS_essl[1] = {0};
static const uint8_t OtTerrainFS_glsl[1] = {0};
static const uint8_t OtTerrainFS_essl[1] = {0};
static const uint8_t OtWaterVS_glsl[1] = {0};
static const uint8_t OtWaterVS_essl[1] = {0};
static const uint8_t OtWaterFS_glsl[1] = {0};
static const uint8_t OtWaterFS_essl[1] = {0};


static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtCubeMapCrossVS),
	BGFX_EMBEDDED_SHADER(OtHdrReprojectVS),
	BGFX_EMBEDDED_SHADER(OtCubeMapCrossFS),
	BGFX_EMBEDDED_SHADER(OtHdrReprojectFS),
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtGeneratorVS),
	BGFX_EMBEDDED_SHADER(OtFbmFS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmFS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtSelectFS),
	BGFX_EMBEDDED_SHADER(OtBrdfLutCS),
	BGFX_EMBEDDED_SHADER(OtIblEnvironmentMapCS),
	BGFX_EMBEDDED_SHADER(OtIblIrradianceMapCS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtWaterVS),
	BGFX_EMBEDDED_SHADER(OtWaterFS),

	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtShaders::get
//

bgfx::ShaderHandle OtShaders::get(const char* name) {
	bgfx::RendererType::Enum type = bgfx::getRendererType();
	auto shader = bgfx::createEmbeddedShader(embeddedShaders, type, name);

	if (!bgfx::isValid(shader)) {
		OtLogFatal(OtFormat("Internal error: Unknown shader [%s]", name));
	}

	return shader;
}