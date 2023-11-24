::	ObjectTalk Scripting Language
::	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
::
::	This work is licensed under the terms of the MIT license.
::	For a copy, see <https://opensource.org/licenses/MIT>.


pushd "%~dp0"

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\deferred/OtDeferredInstancingVS.glsl -o generated\deferred\OtDeferredInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\deferred/OtDeferredLightingVS.glsl -o generated\deferred\OtDeferredLightingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredLightingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\deferred/OtDeferredVS.glsl -o generated\deferred\OtDeferredVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\deferred/OtDeferredLightingFS.glsl -o generated\deferred\OtDeferredLightingFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredLightingFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\deferred/OtDeferredPbrFS.glsl -o generated\deferred\OtDeferredPbrFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredPbrFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f deferred\deferred/OtDeferredTerrainFS.glsl -o generated\deferred\OtDeferredTerrainFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtDeferredTerrainFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtFilterVS.glsl -o generated\filter\OtFilterVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtFilterVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtBlitFS.glsl -o generated\filter\OtBlitFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBlitFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtBloomDownSampleFS.glsl -o generated\filter\OtBloomDownSampleFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomDownSampleFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtBloomFS.glsl -o generated\filter\OtBloomFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtBloomUpSampleFS.glsl -o generated\filter\OtBloomUpSampleFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBloomUpSampleFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtBlurFS.glsl -o generated\filter\OtBlurFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtBlurFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f filter\filter/OtPostProcessFS.glsl -o generated\filter\OtPostProcessFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtPostProcessFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f forward\forward/OtForwardInstancingVS.glsl -o generated\forward\OtForwardInstancingVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtForwardInstancingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f forward\forward/OtForwardVS.glsl -o generated\forward\OtForwardVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtForwardVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f forward\forward/OtForwardPbrFS.glsl -o generated\forward\OtForwardPbrFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtForwardPbrFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f generator\generator/OtGeneratorVS.glsl -o generated\generator\OtGeneratorVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtGeneratorVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f generator\generator/OtTileableFbmFS.glsl -o generated\generator\OtTileableFbmFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtTileableFbmFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f grid\grid/OtGridVS.glsl -o generated\grid\OtGridVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtGridVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f grid\grid/OtGridFS.glsl -o generated\grid\OtGridFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtGridFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\highlight/OtOutlineVS.glsl -o generated\highlight\OtOutlineVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtOutlineVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\highlight/OtSelectVS.glsl -o generated\highlight\OtSelectVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtSelectVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\highlight/OtOutlineFS.glsl -o generated\highlight\OtOutlineFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtOutlineFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f highlight\highlight/OtSelectFS.glsl -o generated\highlight\OtSelectFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSelectFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f imgui\imgui/OtImGuiVS.glsl -o generated\imgui\OtImGuiVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtImGuiVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f imgui\imgui/OtImGuiFS.glsl -o generated\imgui\OtImGuiFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtImGuiFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\sky/OtSkyVS.glsl -o generated\sky\OtSkyVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtSkyVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\sky/OtSkyBoxFS.glsl -o generated\sky\OtSkyBoxFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSkyBoxFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\sky/OtSkyFS.glsl -o generated\sky\OtSkyFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSkyFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f sky\sky/OtSkySphereFS.glsl -o generated\sky\OtSkySphereFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtSkySphereFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f terrain\terrain/OtTerrainVS.glsl -o generated\terrain\OtTerrainVS_dx11.h -i include --type vertex --platform windows --profile s_5_0 -O 3 --bin2c OtTerrainVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f terrain\terrain/OtTerrainFS.glsl -o generated\terrain\OtTerrainFS_dx11.h -i include --type fragment --platform windows --profile s_5_0 -O 3 --bin2c OtTerrainFS_dx11

popd
