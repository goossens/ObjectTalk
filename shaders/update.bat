::	ObjectTalk Scripting Language
::	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
::
::	This work is licensed under the terms of the MIT license.
::	For a copy, see <https://opensource.org/licenses/MIT>.


pushd "%~dp0"

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f canvas\OtCanvasVS.glsl -o generated\canvas\OtCanvasVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtCanvasVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f canvas\OtVectorDisplayVS.glsl -o generated\canvas\OtVectorDisplayVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtVectorDisplayVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f canvas\OtCanvasFS.glsl -o generated\canvas\OtCanvasFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtCanvasFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f canvas\OtVectorDisplayFS.glsl -o generated\canvas\OtVectorDisplayFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtVectorDisplayFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f cubemap\OtCubeMapCrossVS.glsl -o generated\cubemap\OtCubeMapCrossVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtCubeMapCrossVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f cubemap\OtHdrReprojectVS.glsl -o generated\cubemap\OtHdrReprojectVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtHdrReprojectVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f cubemap\OtCubeMapCrossFS.glsl -o generated\cubemap\OtCubeMapCrossFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtCubeMapCrossFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f cubemap\OtHdrReprojectFS.glsl -o generated\cubemap\OtHdrReprojectFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtHdrReprojectFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\OtDeferredInstancingVS.glsl -o generated\deferred\OtDeferredInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\OtDeferredLightingVS.glsl -o generated\deferred\OtDeferredLightingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredLightingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\OtDeferredVS.glsl -o generated\deferred\OtDeferredVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\OtDeferredLightingFS.glsl -o generated\deferred\OtDeferredLightingFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredLightingFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\OtDeferredPbrFS.glsl -o generated\deferred\OtDeferredPbrFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredPbrFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f dial\OtDialVS.glsl -o generated\dial\OtDialVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDialVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f dial\OtDialFS.glsl -o generated\dial\OtDialFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtDialFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtFilterVS.glsl -o generated\filter\OtFilterVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtFilterVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtBlitFS.glsl -o generated\filter\OtBlitFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBlitFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtBloomApplyFS.glsl -o generated\filter\OtBloomApplyFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomApplyFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtBloomDownSampleFS.glsl -o generated\filter\OtBloomDownSampleFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomDownSampleFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtBloomFS.glsl -o generated\filter\OtBloomFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtBloomUpSampleFS.glsl -o generated\filter\OtBloomUpSampleFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomUpSampleFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtBlurFS.glsl -o generated\filter\OtBlurFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBlurFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtFogFS.glsl -o generated\filter\OtFogFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtFogFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtFxaaFS.glsl -o generated\filter\OtFxaaFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtFxaaFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtIslandizerFS.glsl -o generated\filter\OtIslandizerFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtIslandizerFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtLinearizeDepthFS.glsl -o generated\filter\OtLinearizeDepthFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtLinearizeDepthFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtNormalMapperFS.glsl -o generated\filter\OtNormalMapperFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtNormalMapperFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtPostProcessFS.glsl -o generated\filter\OtPostProcessFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtPostProcessFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\OtSeamlessTileFS.glsl -o generated\filter\OtSeamlessTileFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSeamlessTileFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f forward\OtForwardInstancingVS.glsl -o generated\forward\OtForwardInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtForwardInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f forward\OtForwardVS.glsl -o generated\forward\OtForwardVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtForwardVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f forward\OtForwardPbrFS.glsl -o generated\forward\OtForwardPbrFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtForwardPbrFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f generator\OtGeneratorVS.glsl -o generated\generator\OtGeneratorVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtGeneratorVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f generator\OtFbmFS.glsl -o generated\generator\OtFbmFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtFbmFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f generator\OtRenderLightFS.glsl -o generated\generator\OtRenderLightFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtRenderLightFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f generator\OtTileableFbmFS.glsl -o generated\generator\OtTileableFbmFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtTileableFbmFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f godrays\OtGodraysVS.glsl -o generated\godrays\OtGodraysVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtGodraysVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f godrays\OtGodraysFS.glsl -o generated\godrays\OtGodraysFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtGodraysFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f grid\OtGridVS.glsl -o generated\grid\OtGridVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtGridVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f grid\OtGridFS.glsl -o generated\grid\OtGridFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtGridFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\OtOutlineVS.glsl -o generated\highlight\OtOutlineVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtOutlineVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\OtSelectInstancingVS.glsl -o generated\highlight\OtSelectInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtSelectInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\OtSelectVS.glsl -o generated\highlight\OtSelectVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtSelectVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\OtOutlineFS.glsl -o generated\highlight\OtOutlineFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtOutlineFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\OtSelectOpaqueFS.glsl -o generated\highlight\OtSelectOpaqueFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSelectOpaqueFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\OtSelectTransparentFS.glsl -o generated\highlight\OtSelectTransparentFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSelectTransparentFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ibl\OtBrdfLutCS.glsl -o generated\ibl\OtBrdfLutCS_dx11.h -i include --type compute --platform windows --profile s_5_0 -O 3 --bin2c OtBrdfLutCS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ibl\OtIblEnvironmentMapCS.glsl -o generated\ibl\OtIblEnvironmentMapCS_dx11.h -i include --type compute --platform windows --profile s_5_0 -O 3 --bin2c OtIblEnvironmentMapCS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ibl\OtIblIrradianceMapCS.glsl -o generated\ibl\OtIblIrradianceMapCS_dx11.h -i include --type compute --platform windows --profile s_5_0 -O 3 --bin2c OtIblIrradianceMapCS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f imgui\OtImGuiVS.glsl -o generated\imgui\OtImGuiVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtImGuiVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f imgui\OtImGuiFS.glsl -o generated\imgui\OtImGuiFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtImGuiFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f occlusion\OtOcclusionInstancingVS.glsl -o generated\occlusion\OtOcclusionInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtOcclusionInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f occlusion\OtOcclusionVS.glsl -o generated\occlusion\OtOcclusionVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtOcclusionVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f occlusion\OtOcclusionOpaqueFS.glsl -o generated\occlusion\OtOcclusionOpaqueFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtOcclusionOpaqueFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f occlusion\OtOcclusionTransparentFS.glsl -o generated\occlusion\OtOcclusionTransparentFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtOcclusionTransparentFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f particles\OtParticlesVS.glsl -o generated\particles\OtParticlesVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtParticlesVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f particles\OtParticlesFS.glsl -o generated\particles\OtParticlesFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtParticlesFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f picking\OtPickingInstancingVS.glsl -o generated\picking\OtPickingInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtPickingInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f picking\OtPickingVS.glsl -o generated\picking\OtPickingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtPickingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f picking\OtPickingOpaqueFS.glsl -o generated\picking\OtPickingOpaqueFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtPickingOpaqueFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f picking\OtPickingTransparentFS.glsl -o generated\picking\OtPickingTransparentFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtPickingTransparentFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f pointlights\OtPointLightsVS.glsl -o generated\pointlights\OtPointLightsVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtPointLightsVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f pointlights\OtPointLightsFS.glsl -o generated\pointlights\OtPointLightsFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtPointLightsFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f shadow\OtShadowInstancingVS.glsl -o generated\shadow\OtShadowInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtShadowInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f shadow\OtShadowVS.glsl -o generated\shadow\OtShadowVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtShadowVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f shadow\OtShadowOpaqueFS.glsl -o generated\shadow\OtShadowOpaqueFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtShadowOpaqueFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f shadow\OtShadowTransparentFS.glsl -o generated\shadow\OtShadowTransparentFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtShadowTransparentFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\OtSkyVS.glsl -o generated\sky\OtSkyVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtSkyVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\OtSkyBoxFS.glsl -o generated\sky\OtSkyBoxFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSkyBoxFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\OtSkyFS.glsl -o generated\sky\OtSkyFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSkyFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f terrain\OtTerrainSimpleVS.glsl -o generated\terrain\OtTerrainSimpleVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtTerrainSimpleVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f terrain\OtTerrainVS.glsl -o generated\terrain\OtTerrainVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtTerrainVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f terrain\OtTerrainFS.glsl -o generated\terrain\OtTerrainFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtTerrainFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f water\OtWaterVS.glsl -o generated\water\OtWaterVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtWaterVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f water\OtWaterFS.glsl -o generated\water\OtWaterFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtWaterFS_dx11

popd
