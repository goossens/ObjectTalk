#	TextEditor - A syntax highlighting text editor for Dear ImGui.
#	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


import os, shutil, sys

destination = "../../../ImGuiColorTextEdit/"


#
#	copyMain
#

def copyMain(src, dest):
	with open(dest, "w") as output:
		with open(src, "r") as input:
			for line in input:
				if (line == '#include <cmath>\n'):
					output.write('#include <cstdint>\n#include <cmath>\n')

				else:
					output.write(line)


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
#	appendLanguage
#

def appendLanguage(filename, skip, name):
	with open(destination + "TextEditor.cpp", "a") as output:
		open(destination + "TextEditor.cpp", "a").write("\n\n#if defined(TEXTEDITOR_INCLUDE_{0})\n".format(name))
		append(filename, skip)
		open(destination + "TextEditor.cpp", "a").write("#endif\n")



#
#	copyExtras
#

def copyExtras(filename):
	with open(destination + "extras/" + filename, "w") as output:
		with open(filename, "r") as input:
			for line in input:
				if (line == '#include "TextEditor.h"\n'):
					output.write('#include "../TextEditor.h"\n')

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
	copyMain("TextEditor.cpp", destination + "TextEditor.cpp")

	append("Cursors.cpp", 12)
	append("Document.cpp", 12)
	append("Transactions.cpp", 12)
	append("Colorizer.cpp", 12)
	append("Bracketeer.cpp", 12)
	append("FindReplace.cpp", 14)
	append("MiniMap.cpp", 14)
	append("LBC.h", 8)
	append("LineBreakTables.h", 26)
	append("LineBreak.cpp", 15)
	append("LineFold.cpp", 15)
	append("TypeSetter.cpp", 12)
	append("AutoComplete.cpp", 14)
	append("CodePointTables.h", 26)
	append("CodePoint.cpp", 17)
	append("Palette.cpp", 12)

	appendLanguage("languages/C.h", 11, "C")
	appendLanguage("languages/Cpp.h", 7, "CPP")
	appendLanguage("languages/Cs.h", 11, "CS")
	appendLanguage("languages/AngelScript.h", 7, "ANGELSCRIPT")
	appendLanguage("languages/Lua.h", 11, "LUA")
	appendLanguage("languages/Python.h", 11, "PYTHON")
	appendLanguage("languages/Glsl.h", 7, "GLSL")
	appendLanguage("languages/Hlsl.h", 7, "HLSL")
	appendLanguage("languages/Json.h", 11, "JSON")
	appendLanguage("languages/Markdown.h", 11, "MARKDOWN")
	appendLanguage("languages/Sql.h", 7, "SQL")

	shutil.copyfile("dtl.h", destination + "dtl.h")
	shutil.copyfile("TextDiff.h", destination + "TextDiff.h")
	shutil.copyfile("TextDiff.cpp", destination + "TextDiff.cpp")

	copyExtras("LspBridge.cpp")
	copyExtras("LspBridge.h")
	copyExtras("TrieAutoComplete.cpp")
	copyExtras("TrieAutoComplete.h")
