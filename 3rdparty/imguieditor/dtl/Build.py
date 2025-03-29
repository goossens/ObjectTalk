#	TextDiff - A syntax highlighting text diff widget for Dear ImGui.
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


import os, re


#
#	append
#

def append(output, filename, skip):
	with open(filename, "r") as input:
		count = 0

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

	pattern = re.compile(r'.*"(.*)"')

	with open("../dtl.h", "w") as output:
		with open("dtl.hpp", "r") as input:
			for line in input:
				if line.startswith("#include"):
					match = pattern.search(line)
					append(output, match[1], 36)

				else:
					output.write(line)
