#!/bin/bash
#
#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

SCRIPT_DIRECTORY=$(dirname "${BASH_SOURCE[0]}")
cd "${SCRIPT_DIRECTORY}/.." || { echo "Failed to change directory"; exit 1; }

cat templates/shaders.h >OtShaders.h

for FILE in $(ls generated/*.cpp | sort -f)
do
	SHADER=$(basename ${FILE} .cpp)
	echo "extern const uint32_t ${SHADER}[];" >>OtShaders.h
	echo "extern const size_t ${SHADER}Size;" >>OtShaders.h
done
