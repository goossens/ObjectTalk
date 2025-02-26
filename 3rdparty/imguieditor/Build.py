#	TextEditor - A syntax highlighting text editor for ImGui
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


import os, shutil, sys

destination = "../../../ImGuiColorTextEdit/"


#
#	append
#

def append(filename, skip):
	count = 0

	with open(destination + "TextEditor.cpp", "a") as output:
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
		print("Target path[" + destination +"] does not exist")
		sys.exit(1)

	shutil.copyfile("TextEditor.h", destination + "TextEditor.h")
	shutil.copyfile("TextEditor.cpp", destination + "TextEditor.cpp")

	append("Cursors.cpp", 12)
	append("Document.cpp", 12)
	append("Transactions.cpp", 12)
	append("Colorizer.cpp", 12)
	append("Bracketeer.cpp", 12)
	append("Tables.h", 23)
	append("CodePoint.cpp", 17)
	append("Language.cpp", 26)

	append("languages/C.h", 9)
	append("languages/Cpp.h", 5)
	append("languages/Cs.h", 9)
	append("languages/AngelScript.h", 5)
	append("languages/Lua.h", 9)
	append("languages/Python.h", 9)
	append("languages/Glsl.h", 5)
	append("languages/Hlsl.h", 5)
	append("languages/Json.h", 9)
	append("languages/Markdown.h", 9)
	append("languages/Sql.h", 5)
