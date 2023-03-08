::	ObjectTalk Scripting Language
::	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
::
::	This work is licensed under the terms of the MIT license.
::	For a copy, see <https://opensource.org/licenses/MIT>.


pushd "%~dp0"

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomDownSample/shader.vs -o generated/BloomDownSample/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtBloomDownSampleVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomDownSample/shader.vs -o generated/BloomDownSample/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBloomDownSampleVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomDownSample/shader.fs -o generated/BloomDownSample/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtBloomDownSampleFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomDownSample/shader.fs -o generated/BloomDownSample/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBloomDownSampleFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomUpSample/shader.vs -o generated/BloomUpSample/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtBloomUpSampleVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomUpSample/shader.vs -o generated/BloomUpSample/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtBloomUpSampleVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomUpSample/shader.fs -o generated/BloomUpSample/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtBloomUpSampleFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f BloomUpSample/shader.fs -o generated/BloomUpSample/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtBloomUpSampleFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Debug/shader.vs -o generated/Debug/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtDebugVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Debug/shader.vs -o generated/Debug/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtDebugVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Debug/shader.fs -o generated/Debug/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtDebugFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Debug/shader.fs -o generated/Debug/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtDebugFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Geometry/shader.vs -o generated/Geometry/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtGeometryVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Geometry/shader.vs -o generated/Geometry/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtGeometryVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Geometry/shader.fs -o generated/Geometry/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtGeometryFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Geometry/shader.fs -o generated/Geometry/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtGeometryFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Grid/shader.vs -o generated/Grid/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtGridVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Grid/shader.vs -o generated/Grid/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtGridVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Grid/shader.fs -o generated/Grid/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtGridFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Grid/shader.fs -o generated/Grid/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtGridFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ImGui/shader.vs -o generated/ImGui/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtImGuiVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ImGui/shader.vs -o generated/ImGui/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtImGuiVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ImGui/shader.fs -o generated/ImGui/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtImGuiFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f ImGui/shader.fs -o generated/ImGui/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtImGuiFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Lighting/shader.vs -o generated/Lighting/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtLightingVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Lighting/shader.vs -o generated/Lighting/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtLightingVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Lighting/shader.fs -o generated/Lighting/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtLightingFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Lighting/shader.fs -o generated/Lighting/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtLightingFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Outline/shader.vs -o generated/Outline/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtOutlineVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Outline/shader.vs -o generated/Outline/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtOutlineVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Outline/shader.fs -o generated/Outline/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtOutlineFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Outline/shader.fs -o generated/Outline/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtOutlineFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f PostProcess/shader.vs -o generated/PostProcess/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtPostProcessVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f PostProcess/shader.vs -o generated/PostProcess/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtPostProcessVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f PostProcess/shader.fs -o generated/PostProcess/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtPostProcessFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f PostProcess/shader.fs -o generated/PostProcess/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtPostProcessFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Select/shader.vs -o generated/Select/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtSelectVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Select/shader.vs -o generated/Select/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSelectVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Select/shader.fs -o generated/Select/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtSelectFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Select/shader.fs -o generated/Select/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSelectFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Sky/shader.vs -o generated/Sky/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtSkyVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Sky/shader.vs -o generated/Sky/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSkyVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Sky/shader.fs -o generated/Sky/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtSkyFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Sky/shader.fs -o generated/Sky/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSkyFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f SkySphere/shader.vs -o generated/SkySphere/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtSkySphereVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f SkySphere/shader.vs -o generated/SkySphere/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSkySphereVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f SkySphere/shader.fs -o generated/SkySphere/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtSkySphereFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f SkySphere/shader.fs -o generated/SkySphere/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSkySphereFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Skybox/shader.vs -o generated/Skybox/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtSkyboxVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Skybox/shader.vs -o generated/Skybox/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtSkyboxVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Skybox/shader.fs -o generated/Skybox/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtSkyboxFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Skybox/shader.fs -o generated/Skybox/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtSkyboxFS_dx11

%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent/shader.vs -o generated/Transparent/vs_dx9.h -i include --type vertex --platform windows -p s_3_0 -O 3 --bin2c OtTransparentVS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent/shader.vs -o generated/Transparent/vs_dx11.h -i include --type vertex --platform windows -p s_5_0 -O 3 --bin2c OtTransparentVS_dx11
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent/shader.fs -o generated/Transparent/fs_dx9.h -i include --type fragment --platform windows -p s_3_0 -O 3 --bin2c OtTransparentFS_dx9
%HOMEDRIVE%%HOMEPATH%\bin\shaderc -f Transparent/shader.fs -o generated/Transparent/fs_dx11.h -i include --type fragment --platform windows -p s_5_0 -O 3 --bin2c OtTransparentFS_dx11

popd
