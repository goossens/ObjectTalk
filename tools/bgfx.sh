#!/bin/bash

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
pwd
make tools

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
cp bgfx/src/bgfx_shader.sh ${OT_HOME}/shaders/include/bgfx.glsl
