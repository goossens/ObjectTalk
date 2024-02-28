#!/bin/bash
#
#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

OT_HOME="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"

mkdir -p ~/tmp/bgfx
cd ~/tmp/bgfx

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

cd bgfx
make build

if [ -f tools/bin/darwin/shaderc ]
then
	mkdir -p ~/bin
	cp tools/bin/darwin/* ~/bin
fi

if [ -f tools/bin/linux/shaderc ]
then
	mkdir -p ~/bin
	cp tools/bin/linux/* ~/bin
fi

cd ..
cp bgfx/src/bgfx_shader.sh ${OT_HOME}/shaders/include/bgfx_shader.glsl
sed 's/bgfx_shader.sh/bgfx_shader.glsl/g' < bgfx/src/bgfx_compute.sh >${OT_HOME}/shaders/include/bgfx_compute.glsl
