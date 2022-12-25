pushd %HOMEDRIVE%%HOMEPATH%

if not exist "tmp" (
	mkdir tmp
)

if not exist "bin" (
	mkdir bin
)

cd tmp

if exist "bx" (
	cd bx
	git pull
	cd ..

) else (
	git clone https://github.com/bkaradzic/bx.git
)

if exist "bimg" (
	cd bimg
	git pull
	cd ..

) else (
	git clone https://github.com/bkaradzic/bimg.git
)

if exist "bgfx" (
	cd bgfx
	git pull
	cd ..

) else (
	git clone https://github.com/bkaradzic/bgfx.git
)

cd bgfx
..\bx\tools\bin\windows\genie --with-tools vs2022
"\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.com" .build\projects\vs2022\bgfx.sln /build Release

copy /B .build\win64_vs2022\bin\geometrycRelease.exe %HOMEPATH%\bin\geometryc.exe
copy /B .build\win64_vs2022\bin\geometryvRelease.exe %HOMEPATH%\bin\geometryv.exe
copy /B .build\win64_vs2022\bin\shadercRelease.exe %HOMEPATH%\bin\shaderc.exe
copy /B .build\win64_vs2022\bin\texturecRelease.exe %HOMEPATH%\bin\texturec.exe
copy /B .build\win64_vs2022\bin\texturevRelease.exe %HOMEPATH%\bin\texturev.exe

popd
