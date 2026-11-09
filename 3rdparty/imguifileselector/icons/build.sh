#!/bin/bash
#
#	FileSelector - A file select dialog for Dear ImGui.
#	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

# sanity checks
if ! command -v fantasticon >/dev/null 2>&1
then
	echo "[fantasticon] could not be found on your system, please install before running this script."
	exit 1
fi

if ! command -v python >/dev/null 2>&1
then
	echo "[python] could not be found on your system, please install before running this script."
	exit 1
fi

# go to script directory
cd "$(dirname "${BASH_SOURCE[0]}")"

# requires: npm install -g fantasticon
echo "Creating truetype font from icon svg files..."
fantasticon src -o build -t ttf -g json -h 24

# convert font data to C++ files
echo "Converting font data to C++ files..."
python build/convert.py
echo "Done"
