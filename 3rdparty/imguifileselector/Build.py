#	FileSelector - A file select dialog for Dear ImGui.
#	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


import glob, os, shutil, sys

destination = "../../../ImGuiFileSelector/"


#
#	copyMain
#

def copyMain(src, dest):
	with open(dest, "w") as output:
		with open(src, "r") as input:
			for line in input:
				if (line == '#include <regex>\n'):
					output.write('#include <regex>\n#include <sstream>\n')

				else:
					output.write(line)


#
#	append
#

def append(filename, skip):
	count = 0

	with open(destination + "FileSelector.cpp", "a") as output:
		with open(filename, "r") as input:
			for line in input:
				if count < skip:
					count += 1

				else:
					output.write(line)


#
#	__main__
#

if __name__ == "__main__":
	os.chdir(os.path.dirname(os.path.abspath(__file__)))

	if not os.path.isdir(destination):
		print("Target path[" + destination + "] does not exist")
		sys.exit(1)

	shutil.copyfile("FileSelector.h", destination + "FileSelector.h")
	copyMain("FileSelector.cpp", destination + "FileSelector.cpp")

	append("OperatingSystem.cpp", 18)
	append("Icons.cpp", 15)
