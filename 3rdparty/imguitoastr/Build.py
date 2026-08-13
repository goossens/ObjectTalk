#	Toastr - A notification system for Dear ImGui.
#	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


import os, shutil, sys

destination = "../../../ImGuiToastr/"


#
#	__main__
#

if __name__ == "__main__":
	os.chdir(os.path.dirname(os.path.abspath(__file__)))

	if not os.path.isdir(destination):
		print("Target path[" + destination +"] does not exist")
		sys.exit(1)

	shutil.copyfile("Toastr.cpp", destination + "Toastr.cpp")
	shutil.copyfile("Toastr.h", destination + "Toastr.h")
