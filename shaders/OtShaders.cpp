//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "OtLog.h"

#include "OtShaders.h"
#include "generated/canvas/OtMapVS_mtl.h"
#include "generated/canvas/OtMapVS_spv.h"
#include "generated/canvas/OtMapVS_dx11.h"
#include "generated/canvas/OtOscilloscopeVS_mtl.h"
#include "generated/canvas/OtOscilloscopeVS_spv.h"
#include "generated/canvas/OtOscilloscopeVS_dx11.h"
#include "generated/canvas/OtMapFS_mtl.h"
#include "generated/canvas/OtMapFS_spv.h"
#include "generated/canvas/OtMapFS_dx11.h"
#include "generated/canvas/OtOscilloscopeFS_mtl.h"
#include "generated/canvas/OtOscilloscopeFS_spv.h"
#include "generated/canvas/OtOscilloscopeFS_dx11.h"
#include "generated/compositing/OtCompositingVS_mtl.h"
#include "generated/compositing/OtCompositingVS_spv.h"
#include "generated/compositing/OtCompositingVS_dx11.h"
#include "generated/compositing/OtCompositingFS_mtl.h"
#include "generated/compositing/OtCompositingFS_spv.h"
#include "generated/compositing/OtCompositingFS_dx11.h"
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
#include "generated/deferred/OtDeferredAnimatedVS_mtl.h"
#include "generated/deferred/OtDeferredAnimatedVS_spv.h"
#include "generated/deferred/OtDeferredAnimatedVS_dx11.h"
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
#include "generated/dial/OtDialVS_mtl.h"
#include "generated/dial/OtDialVS_spv.h"
#include "generated/dial/OtDialVS_dx11.h"
#include "generated/dial/OtDialFS_mtl.h"
#include "generated/dial/OtDialFS_spv.h"
#include "generated/dial/OtDialFS_dx11.h"
#include "generated/filter/OtFilterVS_mtl.h"
#include "generated/filter/OtFilterVS_spv.h"
#include "generated/filter/OtFilterVS_dx11.h"
#include "generated/filter/OtBloomApplyFS_mtl.h"
#include "generated/filter/OtBloomApplyFS_spv.h"
#include "generated/filter/OtBloomApplyFS_dx11.h"
#include "generated/filter/OtBloomDownSampleFS_mtl.h"
#include "generated/filter/OtBloomDownSampleFS_spv.h"
#include "generated/filter/OtBloomDownSampleFS_dx11.h"
#include "generated/filter/OtBloomUpSampleFS_mtl.h"
#include "generated/filter/OtBloomUpSampleFS_spv.h"
#include "generated/filter/OtBloomUpSampleFS_dx11.h"
#include "generated/filter/OtBlurFS_mtl.h"
#include "generated/filter/OtBlurFS_spv.h"
#include "generated/filter/OtBlurFS_dx11.h"
#include "generated/filter/OtContrastSaturationBrightnessFS_mtl.h"
#include "generated/filter/OtContrastSaturationBrightnessFS_spv.h"
#include "generated/filter/OtContrastSaturationBrightnessFS_dx11.h"
#include "generated/filter/OtFogFS_mtl.h"
#include "generated/filter/OtFogFS_spv.h"
#include "generated/filter/OtFogFS_dx11.h"
#include "generated/filter/OtFxaaFS_mtl.h"
#include "generated/filter/OtFxaaFS_spv.h"
#include "generated/filter/OtFxaaFS_dx11.h"
#include "generated/filter/OtGaussianFS_mtl.h"
#include "generated/filter/OtGaussianFS_spv.h"
#include "generated/filter/OtGaussianFS_dx11.h"
#include "generated/filter/OtIslandizerFS_mtl.h"
#include "generated/filter/OtIslandizerFS_spv.h"
#include "generated/filter/OtIslandizerFS_dx11.h"
#include "generated/filter/OtNormalMapperFS_mtl.h"
#include "generated/filter/OtNormalMapperFS_spv.h"
#include "generated/filter/OtNormalMapperFS_dx11.h"
#include "generated/filter/OtPixelateFS_mtl.h"
#include "generated/filter/OtPixelateFS_spv.h"
#include "generated/filter/OtPixelateFS_dx11.h"
#include "generated/filter/OtPosterizeFS_mtl.h"
#include "generated/filter/OtPosterizeFS_spv.h"
#include "generated/filter/OtPosterizeFS_dx11.h"
#include "generated/filter/OtPostProcessFS_mtl.h"
#include "generated/filter/OtPostProcessFS_spv.h"
#include "generated/filter/OtPostProcessFS_dx11.h"
#include "generated/filter/OtRgbaCurvesFS_mtl.h"
#include "generated/filter/OtRgbaCurvesFS_spv.h"
#include "generated/filter/OtRgbaCurvesFS_dx11.h"
#include "generated/filter/OtRgbaFS_mtl.h"
#include "generated/filter/OtRgbaFS_spv.h"
#include "generated/filter/OtRgbaFS_dx11.h"
#include "generated/filter/OtSeamlessTileFS_mtl.h"
#include "generated/filter/OtSeamlessTileFS_spv.h"
#include "generated/filter/OtSeamlessTileFS_dx11.h"
#include "generated/filter/OtSharpenFS_mtl.h"
#include "generated/filter/OtSharpenFS_spv.h"
#include "generated/filter/OtSharpenFS_dx11.h"
#include "generated/forward/OtForwardInstancingVS_mtl.h"
#include "generated/forward/OtForwardInstancingVS_spv.h"
#include "generated/forward/OtForwardInstancingVS_dx11.h"
#include "generated/forward/OtForwardVS_mtl.h"
#include "generated/forward/OtForwardVS_spv.h"
#include "generated/forward/OtForwardVS_dx11.h"
#include "generated/forward/OtForwardPbrFS_mtl.h"
#include "generated/forward/OtForwardPbrFS_spv.h"
#include "generated/forward/OtForwardPbrFS_dx11.h"
#include "generated/generator/OtCheckerBoardCS_mtl.h"
#include "generated/generator/OtCheckerBoardCS_spv.h"
#include "generated/generator/OtCheckerBoardCS_dx11.h"
#include "generated/generator/OtColorWheelCS_mtl.h"
#include "generated/generator/OtColorWheelCS_spv.h"
#include "generated/generator/OtColorWheelCS_dx11.h"
#include "generated/generator/OtFbmCS_mtl.h"
#include "generated/generator/OtFbmCS_spv.h"
#include "generated/generator/OtFbmCS_dx11.h"
#include "generated/generator/OtRenderLightCS_mtl.h"
#include "generated/generator/OtRenderLightCS_spv.h"
#include "generated/generator/OtRenderLightCS_dx11.h"
#include "generated/generator/OtTileableFbmCS_mtl.h"
#include "generated/generator/OtTileableFbmCS_spv.h"
#include "generated/generator/OtTileableFbmCS_dx11.h"
#include "generated/godrays/OtGodraysVS_mtl.h"
#include "generated/godrays/OtGodraysVS_spv.h"
#include "generated/godrays/OtGodraysVS_dx11.h"
#include "generated/godrays/OtGodraysFS_mtl.h"
#include "generated/godrays/OtGodraysFS_spv.h"
#include "generated/godrays/OtGodraysFS_dx11.h"
#include "generated/grass/OtGrassSimpleVS_mtl.h"
#include "generated/grass/OtGrassSimpleVS_spv.h"
#include "generated/grass/OtGrassSimpleVS_dx11.h"
#include "generated/grass/OtGrassVS_mtl.h"
#include "generated/grass/OtGrassVS_spv.h"
#include "generated/grass/OtGrassVS_dx11.h"
#include "generated/grass/OtGrassFS_mtl.h"
#include "generated/grass/OtGrassFS_spv.h"
#include "generated/grass/OtGrassFS_dx11.h"
#include "generated/grid/OtGridVS_mtl.h"
#include "generated/grid/OtGridVS_spv.h"
#include "generated/grid/OtGridVS_dx11.h"
#include "generated/grid/OtGridFS_mtl.h"
#include "generated/grid/OtGridFS_spv.h"
#include "generated/grid/OtGridFS_dx11.h"
#include "generated/highlight/OtOutlineVS_mtl.h"
#include "generated/highlight/OtOutlineVS_spv.h"
#include "generated/highlight/OtOutlineVS_dx11.h"
#include "generated/highlight/OtSelectAnimatedVS_mtl.h"
#include "generated/highlight/OtSelectAnimatedVS_spv.h"
#include "generated/highlight/OtSelectAnimatedVS_dx11.h"
#include "generated/highlight/OtSelectInstancingVS_mtl.h"
#include "generated/highlight/OtSelectInstancingVS_spv.h"
#include "generated/highlight/OtSelectInstancingVS_dx11.h"
#include "generated/highlight/OtSelectVS_mtl.h"
#include "generated/highlight/OtSelectVS_spv.h"
#include "generated/highlight/OtSelectVS_dx11.h"
#include "generated/highlight/OtOutlineFS_mtl.h"
#include "generated/highlight/OtOutlineFS_spv.h"
#include "generated/highlight/OtOutlineFS_dx11.h"
#include "generated/highlight/OtSelectOpaqueFS_mtl.h"
#include "generated/highlight/OtSelectOpaqueFS_spv.h"
#include "generated/highlight/OtSelectOpaqueFS_dx11.h"
#include "generated/highlight/OtSelectTransparentFS_mtl.h"
#include "generated/highlight/OtSelectTransparentFS_spv.h"
#include "generated/highlight/OtSelectTransparentFS_dx11.h"
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
#include "generated/occlusion/OtOcclusionAnimatedVS_mtl.h"
#include "generated/occlusion/OtOcclusionAnimatedVS_spv.h"
#include "generated/occlusion/OtOcclusionAnimatedVS_dx11.h"
#include "generated/occlusion/OtOcclusionInstancingVS_mtl.h"
#include "generated/occlusion/OtOcclusionInstancingVS_spv.h"
#include "generated/occlusion/OtOcclusionInstancingVS_dx11.h"
#include "generated/occlusion/OtOcclusionVS_mtl.h"
#include "generated/occlusion/OtOcclusionVS_spv.h"
#include "generated/occlusion/OtOcclusionVS_dx11.h"
#include "generated/occlusion/OtOcclusionOpaqueFS_mtl.h"
#include "generated/occlusion/OtOcclusionOpaqueFS_spv.h"
#include "generated/occlusion/OtOcclusionOpaqueFS_dx11.h"
#include "generated/occlusion/OtOcclusionTransparentFS_mtl.h"
#include "generated/occlusion/OtOcclusionTransparentFS_spv.h"
#include "generated/occlusion/OtOcclusionTransparentFS_dx11.h"
#include "generated/particles/OtParticlesVS_mtl.h"
#include "generated/particles/OtParticlesVS_spv.h"
#include "generated/particles/OtParticlesVS_dx11.h"
#include "generated/particles/OtParticlesFS_mtl.h"
#include "generated/particles/OtParticlesFS_spv.h"
#include "generated/particles/OtParticlesFS_dx11.h"
#include "generated/picking/OtPickingAnimatedVS_mtl.h"
#include "generated/picking/OtPickingAnimatedVS_spv.h"
#include "generated/picking/OtPickingAnimatedVS_dx11.h"
#include "generated/picking/OtPickingInstancingVS_mtl.h"
#include "generated/picking/OtPickingInstancingVS_spv.h"
#include "generated/picking/OtPickingInstancingVS_dx11.h"
#include "generated/picking/OtPickingVS_mtl.h"
#include "generated/picking/OtPickingVS_spv.h"
#include "generated/picking/OtPickingVS_dx11.h"
#include "generated/picking/OtPickingOpaqueFS_mtl.h"
#include "generated/picking/OtPickingOpaqueFS_spv.h"
#include "generated/picking/OtPickingOpaqueFS_dx11.h"
#include "generated/picking/OtPickingTransparentFS_mtl.h"
#include "generated/picking/OtPickingTransparentFS_spv.h"
#include "generated/picking/OtPickingTransparentFS_dx11.h"
#include "generated/pointlights/OtPointLightsVS_mtl.h"
#include "generated/pointlights/OtPointLightsVS_spv.h"
#include "generated/pointlights/OtPointLightsVS_dx11.h"
#include "generated/pointlights/OtPointLightsFS_mtl.h"
#include "generated/pointlights/OtPointLightsFS_spv.h"
#include "generated/pointlights/OtPointLightsFS_dx11.h"
#include "generated/preview/OtPreviewVS_mtl.h"
#include "generated/preview/OtPreviewVS_spv.h"
#include "generated/preview/OtPreviewVS_dx11.h"
#include "generated/preview/OtPreviewFS_mtl.h"
#include "generated/preview/OtPreviewFS_spv.h"
#include "generated/preview/OtPreviewFS_dx11.h"
#include "generated/shadow/OtShadowAnimatedVS_mtl.h"
#include "generated/shadow/OtShadowAnimatedVS_spv.h"
#include "generated/shadow/OtShadowAnimatedVS_dx11.h"
#include "generated/shadow/OtShadowInstancingVS_mtl.h"
#include "generated/shadow/OtShadowInstancingVS_spv.h"
#include "generated/shadow/OtShadowInstancingVS_dx11.h"
#include "generated/shadow/OtShadowVS_mtl.h"
#include "generated/shadow/OtShadowVS_spv.h"
#include "generated/shadow/OtShadowVS_dx11.h"
#include "generated/shadow/OtShadowOpaqueFS_mtl.h"
#include "generated/shadow/OtShadowOpaqueFS_spv.h"
#include "generated/shadow/OtShadowOpaqueFS_dx11.h"
#include "generated/shadow/OtShadowTransparentFS_mtl.h"
#include "generated/shadow/OtShadowTransparentFS_spv.h"
#include "generated/shadow/OtShadowTransparentFS_dx11.h"
#include "generated/sky/OtSkyVS_mtl.h"
#include "generated/sky/OtSkyVS_spv.h"
#include "generated/sky/OtSkyVS_dx11.h"
#include "generated/sky/OtSkyBoxFS_mtl.h"
#include "generated/sky/OtSkyBoxFS_spv.h"
#include "generated/sky/OtSkyBoxFS_dx11.h"
#include "generated/sky/OtSkyFS_mtl.h"
#include "generated/sky/OtSkyFS_spv.h"
#include "generated/sky/OtSkyFS_dx11.h"
#include "generated/terrain/OtTerrainSimpleVS_mtl.h"
#include "generated/terrain/OtTerrainSimpleVS_spv.h"
#include "generated/terrain/OtTerrainSimpleVS_dx11.h"
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

static const uint8_t OtMapVS_glsl[1] = {0};
static const uint8_t OtMapVS_essl[1] = {0};
static const uint8_t OtOscilloscopeVS_glsl[1] = {0};
static const uint8_t OtOscilloscopeVS_essl[1] = {0};
static const uint8_t OtMapFS_glsl[1] = {0};
static const uint8_t OtMapFS_essl[1] = {0};
static const uint8_t OtOscilloscopeFS_glsl[1] = {0};
static const uint8_t OtOscilloscopeFS_essl[1] = {0};
static const uint8_t OtCompositingVS_glsl[1] = {0};
static const uint8_t OtCompositingVS_essl[1] = {0};
static const uint8_t OtCompositingFS_glsl[1] = {0};
static const uint8_t OtCompositingFS_essl[1] = {0};
static const uint8_t OtCubeMapCrossVS_glsl[1] = {0};
static const uint8_t OtCubeMapCrossVS_essl[1] = {0};
static const uint8_t OtHdrReprojectVS_glsl[1] = {0};
static const uint8_t OtHdrReprojectVS_essl[1] = {0};
static const uint8_t OtCubeMapCrossFS_glsl[1] = {0};
static const uint8_t OtCubeMapCrossFS_essl[1] = {0};
static const uint8_t OtHdrReprojectFS_glsl[1] = {0};
static const uint8_t OtHdrReprojectFS_essl[1] = {0};
static const uint8_t OtDeferredAnimatedVS_glsl[1] = {0};
static const uint8_t OtDeferredAnimatedVS_essl[1] = {0};
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
static const uint8_t OtDialVS_glsl[1] = {0};
static const uint8_t OtDialVS_essl[1] = {0};
static const uint8_t OtDialFS_glsl[1] = {0};
static const uint8_t OtDialFS_essl[1] = {0};
static const uint8_t OtFilterVS_glsl[1] = {0};
static const uint8_t OtFilterVS_essl[1] = {0};
static const uint8_t OtBloomApplyFS_glsl[1] = {0};
static const uint8_t OtBloomApplyFS_essl[1] = {0};
static const uint8_t OtBloomDownSampleFS_glsl[1] = {0};
static const uint8_t OtBloomDownSampleFS_essl[1] = {0};
static const uint8_t OtBloomUpSampleFS_glsl[1] = {0};
static const uint8_t OtBloomUpSampleFS_essl[1] = {0};
static const uint8_t OtBlurFS_glsl[1] = {0};
static const uint8_t OtBlurFS_essl[1] = {0};
static const uint8_t OtContrastSaturationBrightnessFS_glsl[1] = {0};
static const uint8_t OtContrastSaturationBrightnessFS_essl[1] = {0};
static const uint8_t OtFogFS_glsl[1] = {0};
static const uint8_t OtFogFS_essl[1] = {0};
static const uint8_t OtFxaaFS_glsl[1] = {0};
static const uint8_t OtFxaaFS_essl[1] = {0};
static const uint8_t OtGaussianFS_glsl[1] = {0};
static const uint8_t OtGaussianFS_essl[1] = {0};
static const uint8_t OtIslandizerFS_glsl[1] = {0};
static const uint8_t OtIslandizerFS_essl[1] = {0};
static const uint8_t OtNormalMapperFS_glsl[1] = {0};
static const uint8_t OtNormalMapperFS_essl[1] = {0};
static const uint8_t OtPixelateFS_glsl[1] = {0};
static const uint8_t OtPixelateFS_essl[1] = {0};
static const uint8_t OtPosterizeFS_glsl[1] = {0};
static const uint8_t OtPosterizeFS_essl[1] = {0};
static const uint8_t OtPostProcessFS_glsl[1] = {0};
static const uint8_t OtPostProcessFS_essl[1] = {0};
static const uint8_t OtRgbaCurvesFS_glsl[1] = {0};
static const uint8_t OtRgbaCurvesFS_essl[1] = {0};
static const uint8_t OtRgbaFS_glsl[1] = {0};
static const uint8_t OtRgbaFS_essl[1] = {0};
static const uint8_t OtSeamlessTileFS_glsl[1] = {0};
static const uint8_t OtSeamlessTileFS_essl[1] = {0};
static const uint8_t OtSharpenFS_glsl[1] = {0};
static const uint8_t OtSharpenFS_essl[1] = {0};
static const uint8_t OtForwardInstancingVS_glsl[1] = {0};
static const uint8_t OtForwardInstancingVS_essl[1] = {0};
static const uint8_t OtForwardVS_glsl[1] = {0};
static const uint8_t OtForwardVS_essl[1] = {0};
static const uint8_t OtForwardPbrFS_glsl[1] = {0};
static const uint8_t OtForwardPbrFS_essl[1] = {0};
static const uint8_t OtCheckerBoardCS_glsl[1] = {0};
static const uint8_t OtCheckerBoardCS_essl[1] = {0};
static const uint8_t OtColorWheelCS_glsl[1] = {0};
static const uint8_t OtColorWheelCS_essl[1] = {0};
static const uint8_t OtFbmCS_glsl[1] = {0};
static const uint8_t OtFbmCS_essl[1] = {0};
static const uint8_t OtRenderLightCS_glsl[1] = {0};
static const uint8_t OtRenderLightCS_essl[1] = {0};
static const uint8_t OtTileableFbmCS_glsl[1] = {0};
static const uint8_t OtTileableFbmCS_essl[1] = {0};
static const uint8_t OtGodraysVS_glsl[1] = {0};
static const uint8_t OtGodraysVS_essl[1] = {0};
static const uint8_t OtGodraysFS_glsl[1] = {0};
static const uint8_t OtGodraysFS_essl[1] = {0};
static const uint8_t OtGrassSimpleVS_glsl[1] = {0};
static const uint8_t OtGrassSimpleVS_essl[1] = {0};
static const uint8_t OtGrassVS_glsl[1] = {0};
static const uint8_t OtGrassVS_essl[1] = {0};
static const uint8_t OtGrassFS_glsl[1] = {0};
static const uint8_t OtGrassFS_essl[1] = {0};
static const uint8_t OtGridVS_glsl[1] = {0};
static const uint8_t OtGridVS_essl[1] = {0};
static const uint8_t OtGridFS_glsl[1] = {0};
static const uint8_t OtGridFS_essl[1] = {0};
static const uint8_t OtOutlineVS_glsl[1] = {0};
static const uint8_t OtOutlineVS_essl[1] = {0};
static const uint8_t OtSelectAnimatedVS_glsl[1] = {0};
static const uint8_t OtSelectAnimatedVS_essl[1] = {0};
static const uint8_t OtSelectInstancingVS_glsl[1] = {0};
static const uint8_t OtSelectInstancingVS_essl[1] = {0};
static const uint8_t OtSelectVS_glsl[1] = {0};
static const uint8_t OtSelectVS_essl[1] = {0};
static const uint8_t OtOutlineFS_glsl[1] = {0};
static const uint8_t OtOutlineFS_essl[1] = {0};
static const uint8_t OtSelectOpaqueFS_glsl[1] = {0};
static const uint8_t OtSelectOpaqueFS_essl[1] = {0};
static const uint8_t OtSelectTransparentFS_glsl[1] = {0};
static const uint8_t OtSelectTransparentFS_essl[1] = {0};
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
static const uint8_t OtOcclusionAnimatedVS_glsl[1] = {0};
static const uint8_t OtOcclusionAnimatedVS_essl[1] = {0};
static const uint8_t OtOcclusionInstancingVS_glsl[1] = {0};
static const uint8_t OtOcclusionInstancingVS_essl[1] = {0};
static const uint8_t OtOcclusionVS_glsl[1] = {0};
static const uint8_t OtOcclusionVS_essl[1] = {0};
static const uint8_t OtOcclusionOpaqueFS_glsl[1] = {0};
static const uint8_t OtOcclusionOpaqueFS_essl[1] = {0};
static const uint8_t OtOcclusionTransparentFS_glsl[1] = {0};
static const uint8_t OtOcclusionTransparentFS_essl[1] = {0};
static const uint8_t OtParticlesVS_glsl[1] = {0};
static const uint8_t OtParticlesVS_essl[1] = {0};
static const uint8_t OtParticlesFS_glsl[1] = {0};
static const uint8_t OtParticlesFS_essl[1] = {0};
static const uint8_t OtPickingAnimatedVS_glsl[1] = {0};
static const uint8_t OtPickingAnimatedVS_essl[1] = {0};
static const uint8_t OtPickingInstancingVS_glsl[1] = {0};
static const uint8_t OtPickingInstancingVS_essl[1] = {0};
static const uint8_t OtPickingVS_glsl[1] = {0};
static const uint8_t OtPickingVS_essl[1] = {0};
static const uint8_t OtPickingOpaqueFS_glsl[1] = {0};
static const uint8_t OtPickingOpaqueFS_essl[1] = {0};
static const uint8_t OtPickingTransparentFS_glsl[1] = {0};
static const uint8_t OtPickingTransparentFS_essl[1] = {0};
static const uint8_t OtPointLightsVS_glsl[1] = {0};
static const uint8_t OtPointLightsVS_essl[1] = {0};
static const uint8_t OtPointLightsFS_glsl[1] = {0};
static const uint8_t OtPointLightsFS_essl[1] = {0};
static const uint8_t OtPreviewVS_glsl[1] = {0};
static const uint8_t OtPreviewVS_essl[1] = {0};
static const uint8_t OtPreviewFS_glsl[1] = {0};
static const uint8_t OtPreviewFS_essl[1] = {0};
static const uint8_t OtShadowAnimatedVS_glsl[1] = {0};
static const uint8_t OtShadowAnimatedVS_essl[1] = {0};
static const uint8_t OtShadowInstancingVS_glsl[1] = {0};
static const uint8_t OtShadowInstancingVS_essl[1] = {0};
static const uint8_t OtShadowVS_glsl[1] = {0};
static const uint8_t OtShadowVS_essl[1] = {0};
static const uint8_t OtShadowOpaqueFS_glsl[1] = {0};
static const uint8_t OtShadowOpaqueFS_essl[1] = {0};
static const uint8_t OtShadowTransparentFS_glsl[1] = {0};
static const uint8_t OtShadowTransparentFS_essl[1] = {0};
static const uint8_t OtSkyVS_glsl[1] = {0};
static const uint8_t OtSkyVS_essl[1] = {0};
static const uint8_t OtSkyBoxFS_glsl[1] = {0};
static const uint8_t OtSkyBoxFS_essl[1] = {0};
static const uint8_t OtSkyFS_glsl[1] = {0};
static const uint8_t OtSkyFS_essl[1] = {0};
static const uint8_t OtTerrainSimpleVS_glsl[1] = {0};
static const uint8_t OtTerrainSimpleVS_essl[1] = {0};
static const uint8_t OtTerrainVS_glsl[1] = {0};
static const uint8_t OtTerrainVS_essl[1] = {0};
static const uint8_t OtTerrainFS_glsl[1] = {0};
static const uint8_t OtTerrainFS_essl[1] = {0};
static const uint8_t OtWaterVS_glsl[1] = {0};
static const uint8_t OtWaterVS_essl[1] = {0};
static const uint8_t OtWaterFS_glsl[1] = {0};
static const uint8_t OtWaterFS_essl[1] = {0};


static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtMapVS),
	BGFX_EMBEDDED_SHADER(OtOscilloscopeVS),
	BGFX_EMBEDDED_SHADER(OtMapFS),
	BGFX_EMBEDDED_SHADER(OtOscilloscopeFS),
	BGFX_EMBEDDED_SHADER(OtCompositingVS),
	BGFX_EMBEDDED_SHADER(OtCompositingFS),
	BGFX_EMBEDDED_SHADER(OtCubeMapCrossVS),
	BGFX_EMBEDDED_SHADER(OtHdrReprojectVS),
	BGFX_EMBEDDED_SHADER(OtCubeMapCrossFS),
	BGFX_EMBEDDED_SHADER(OtHdrReprojectFS),
	BGFX_EMBEDDED_SHADER(OtDeferredAnimatedVS),
	BGFX_EMBEDDED_SHADER(OtDeferredInstancingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingVS),
	BGFX_EMBEDDED_SHADER(OtDeferredVS),
	BGFX_EMBEDDED_SHADER(OtDeferredLightingFS),
	BGFX_EMBEDDED_SHADER(OtDeferredPbrFS),
	BGFX_EMBEDDED_SHADER(OtDialVS),
	BGFX_EMBEDDED_SHADER(OtDialFS),
	BGFX_EMBEDDED_SHADER(OtFilterVS),
	BGFX_EMBEDDED_SHADER(OtBloomApplyFS),
	BGFX_EMBEDDED_SHADER(OtBloomDownSampleFS),
	BGFX_EMBEDDED_SHADER(OtBloomUpSampleFS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER(OtContrastSaturationBrightnessFS),
	BGFX_EMBEDDED_SHADER(OtFogFS),
	BGFX_EMBEDDED_SHADER(OtFxaaFS),
	BGFX_EMBEDDED_SHADER(OtGaussianFS),
	BGFX_EMBEDDED_SHADER(OtIslandizerFS),
	BGFX_EMBEDDED_SHADER(OtNormalMapperFS),
	BGFX_EMBEDDED_SHADER(OtPixelateFS),
	BGFX_EMBEDDED_SHADER(OtPosterizeFS),
	BGFX_EMBEDDED_SHADER(OtPostProcessFS),
	BGFX_EMBEDDED_SHADER(OtRgbaCurvesFS),
	BGFX_EMBEDDED_SHADER(OtRgbaFS),
	BGFX_EMBEDDED_SHADER(OtSeamlessTileFS),
	BGFX_EMBEDDED_SHADER(OtSharpenFS),
	BGFX_EMBEDDED_SHADER(OtForwardInstancingVS),
	BGFX_EMBEDDED_SHADER(OtForwardVS),
	BGFX_EMBEDDED_SHADER(OtForwardPbrFS),
	BGFX_EMBEDDED_SHADER(OtCheckerBoardCS),
	BGFX_EMBEDDED_SHADER(OtColorWheelCS),
	BGFX_EMBEDDED_SHADER(OtFbmCS),
	BGFX_EMBEDDED_SHADER(OtRenderLightCS),
	BGFX_EMBEDDED_SHADER(OtTileableFbmCS),
	BGFX_EMBEDDED_SHADER(OtGodraysVS),
	BGFX_EMBEDDED_SHADER(OtGodraysFS),
	BGFX_EMBEDDED_SHADER(OtGrassSimpleVS),
	BGFX_EMBEDDED_SHADER(OtGrassVS),
	BGFX_EMBEDDED_SHADER(OtGrassFS),
	BGFX_EMBEDDED_SHADER(OtGridVS),
	BGFX_EMBEDDED_SHADER(OtGridFS),
	BGFX_EMBEDDED_SHADER(OtOutlineVS),
	BGFX_EMBEDDED_SHADER(OtSelectAnimatedVS),
	BGFX_EMBEDDED_SHADER(OtSelectInstancingVS),
	BGFX_EMBEDDED_SHADER(OtSelectVS),
	BGFX_EMBEDDED_SHADER(OtOutlineFS),
	BGFX_EMBEDDED_SHADER(OtSelectOpaqueFS),
	BGFX_EMBEDDED_SHADER(OtSelectTransparentFS),
	BGFX_EMBEDDED_SHADER(OtBrdfLutCS),
	BGFX_EMBEDDED_SHADER(OtIblEnvironmentMapCS),
	BGFX_EMBEDDED_SHADER(OtIblIrradianceMapCS),
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER(OtOcclusionAnimatedVS),
	BGFX_EMBEDDED_SHADER(OtOcclusionInstancingVS),
	BGFX_EMBEDDED_SHADER(OtOcclusionVS),
	BGFX_EMBEDDED_SHADER(OtOcclusionOpaqueFS),
	BGFX_EMBEDDED_SHADER(OtOcclusionTransparentFS),
	BGFX_EMBEDDED_SHADER(OtParticlesVS),
	BGFX_EMBEDDED_SHADER(OtParticlesFS),
	BGFX_EMBEDDED_SHADER(OtPickingAnimatedVS),
	BGFX_EMBEDDED_SHADER(OtPickingInstancingVS),
	BGFX_EMBEDDED_SHADER(OtPickingVS),
	BGFX_EMBEDDED_SHADER(OtPickingOpaqueFS),
	BGFX_EMBEDDED_SHADER(OtPickingTransparentFS),
	BGFX_EMBEDDED_SHADER(OtPointLightsVS),
	BGFX_EMBEDDED_SHADER(OtPointLightsFS),
	BGFX_EMBEDDED_SHADER(OtPreviewVS),
	BGFX_EMBEDDED_SHADER(OtPreviewFS),
	BGFX_EMBEDDED_SHADER(OtShadowAnimatedVS),
	BGFX_EMBEDDED_SHADER(OtShadowInstancingVS),
	BGFX_EMBEDDED_SHADER(OtShadowVS),
	BGFX_EMBEDDED_SHADER(OtShadowOpaqueFS),
	BGFX_EMBEDDED_SHADER(OtShadowTransparentFS),
	BGFX_EMBEDDED_SHADER(OtSkyVS),
	BGFX_EMBEDDED_SHADER(OtSkyBoxFS),
	BGFX_EMBEDDED_SHADER(OtSkyFS),
	BGFX_EMBEDDED_SHADER(OtTerrainSimpleVS),
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
		OtLogFatal("Internal error: unknown shader [{}]", name);
	}

	return shader;
}
