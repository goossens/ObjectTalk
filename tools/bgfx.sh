#!/bin/bash
#
#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

OT_HOME="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"

mkdir -p ~/bin
mkdir -p ~/tmp/bgfx
cd ~/tmp/bgfx

if [ -d genie ]
then
	cd genie
	git pull
	cd ..

else
	git clone https://github.com/bkaradzic/genie.git
fi

if [ -d bx ]
then
	cd bx
	git pull
	cd ..

else
	git clone https://github.com/bkaradzic/bx.git
fi

if [ -d bimg ]
then
	cd bimg
	git pull
	cd ..

else
	git clone https://github.com/bkaradzic/bimg.git
fi

if [ -d bgfx ]
then
	cd bgfx
	git pull
	cd ..

else
	git clone https://github.com/bkaradzic/bgfx.git
fi

cd genie
make
cd  ..

cd bgfx

case $(uname | tr '[:upper:]' '[:lower:]') in
	darwin*)
		../genie/bin/darwin/genie --with-tools --with-combined-examples --with-shared-lib --gcc=osx-x64 gmake
		make -C .build/projects/gmake-osx-x64 config=release
		cp ".build/osx-x64/bin/geometrycRelease" ~/bin/geometryc
		cp ".build/osx-x64/bin/geometryvRelease" ~/bin/geometryv
		cp ".build/osx-x64/bin/shadercRelease" ~/bin/shaderc
		cp ".build/osx-x64/bin/texturecRelease" ~/bin/texturec
		cp ".build/osx-x64/bin/texturevRelease" ~/bin/texturev
    	;;

	linux*)
		../genie/bin/darwin/genie --with-tools --with-combined-examples --with-shared-lib --gcc=linux-gcc gmake
		make -C .build/projects/gmake-linux-gcc config=release64
		cp ".build/linux-gcc/bin/geometrycRelease" ~/bin/geometryc
		cp ".build/linux-gcc/bin/geometryvRelease" ~/bin/geometryv
		cp ".build/linux-gcc/bin/shadercRelease" ~/bin/shaderc
		cp ".build/linux-gcc/bin/texturecRelease" ~/bin/texturec
		cp ".build/linux-gcc/bin/texturevRelease" ~/bin/texturev
		;;
esac

cp src/bgfx_shader.sh ${OT_HOME}/shaders/include/bgfx_shader.glsl
sed 's/bgfx_shader.sh/bgfx_shader.glsl/g' < src/bgfx_compute.sh >${OT_HOME}/shaders/include/bgfx_compute.glsl
