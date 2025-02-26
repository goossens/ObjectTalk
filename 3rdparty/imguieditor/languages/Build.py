#	TextEditor - A syntax highlighting text editor for Dear ImGui.
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


import glob, os


#
#	__main__
#

if __name__ == "__main__":
	os.chdir(os.path.dirname(os.path.abspath(__file__)))

	for rePath in glob.iglob("*.re"):
		hPath = os.path.splitext(rePath)[0] + ".h"

		if not os.path.exists(hPath) or os.path.getmtime(rePath) > os.path.getmtime(hPath):
			command = f"re2c -i -o {hPath} {rePath}"
			print(command)
			os.system(command)
