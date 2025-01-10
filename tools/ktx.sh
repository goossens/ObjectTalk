#!/bin/bash
#
#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

OT_HOME="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"

cd ~/tmp/

if [ -d KTX-Software ]
then
	cd KTX-Software
	git pull

else
	git clone https://github.com/KhronosGroup/KTX-Software
	cd KTX-Software
fi

cmake . -B build -D KTX_FEATURE_STATIC_LIBRARY=ON
cmake --build build
cd build

cp ktxinfo toktx ~/bin
