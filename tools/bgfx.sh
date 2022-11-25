#!/bin/bash

mkdir -p ~/tmp/bgfx
cd ~/tmp/bgfx

if [ ! -d bx ]
then
	git clone https://github.com/bkaradzic/bx.git
fi

if [ ! -d bimg ]
then
	git clone https://github.com/bkaradzic/bimg.git
fi

if [ ! -d bgfx ]
then
	git clone https://github.com/bkaradzic/bgfx.git
fi

cd bgfx
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
