::	ObjectTalk Scripting Language
::	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
::
::	This work is licensed under the terms of the MIT license.
::	For a copy, see <https://opensource.org/licenses/MIT>.


pushd "%~dp0"

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Blit\shader.vs -o generated\Blit\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBlitVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Blit\shader.fs -o generated\Blit\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBlitFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Bloom\shader.vs -o generated\Bloom\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBloomVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Bloom\shader.fs -o generated\Bloom\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBloomFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomDownSample\shader.vs -o generated\BloomDownSample\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBloomDownSampleVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomDownSample\shader.fs -o generated\BloomDownSample\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBloomDownSampleFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomUpSample\shader.vs -o generated\BloomUpSample\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBloomUpSampleVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomUpSample\shader.fs -o generated\BloomUpSample\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBloomUpSampleFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Blur\shader.vs -o generated\Blur\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBlurVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Blur\shader.fs -o generated\Blur\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBlurFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Debug\shader.vs -o generated\Debug\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtDebugVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Debug\shader.fs -o generated\Debug\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtDebugFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f GeometryPbr\shader.vs -o generated\GeometryPbr\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtGeometryPbrVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f GeometryPbr\shader.fs -o generated\GeometryPbr\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtGeometryPbrFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f GeometryPbr\instancing.vs -o generated\GeometryPbr\vsi_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtGeometryPbrVSI_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f GeometryTerrain\shader.vs -o generated\GeometryTerrain\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtGeometryTerrainVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f GeometryTerrain\shader.fs -o generated\GeometryTerrain\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtGeometryTerrainFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Grid\shader.vs -o generated\Grid\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtGridVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Grid\shader.fs -o generated\Grid\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtGridFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ImGui\shader.vs -o generated\ImGui\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtImGuiVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ImGui\shader.fs -o generated\ImGui\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtImGuiFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Lighting\shader.vs -o generated\Lighting\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtLightingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Lighting\shader.fs -o generated\Lighting\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtLightingFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Outline\shader.vs -o generated\Outline\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtOutlineVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Outline\shader.fs -o generated\Outline\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtOutlineFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f PostProcess\shader.vs -o generated\PostProcess\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtPostProcessVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f PostProcess\shader.fs -o generated\PostProcess\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtPostProcessFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Select\shader.vs -o generated\Select\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSelectVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Select\shader.fs -o generated\Select\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSelectFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Sky\shader.vs -o generated\Sky\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSkyVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Sky\shader.fs -o generated\Sky\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSkyFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f SkySphere\shader.vs -o generated\SkySphere\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSkySphereVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f SkySphere\shader.fs -o generated\SkySphere\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSkySphereFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Skybox\shader.vs -o generated\Skybox\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSkyboxVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Skybox\shader.fs -o generated\Skybox\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSkyboxFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent\shader.vs -o generated\Transparent\vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtTransparentVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent\shader.fs -o generated\Transparent\fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtTransparentFS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent\instancing.vs -o generated\Transparent\vsi_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtTransparentVSI_dx11

popd
