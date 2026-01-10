#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

import glob, os, pathlib, subprocess, sys

# switch directory to script location
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# read standard header
with open("OtFaustHeader.h", "r") as file:
	header = file.read()

# process all DSP files and generate code
for sourceFileName in glob.iglob("*.dsp"):
	className = pathlib.Path(sourceFileName).stem

	# run faust to generate C++ code
	command = [
		"faust",
		"--language", "cpp",
		"--double-precision-floats",
		"--class-name", className,
		"--super-class-name", "OtFaust",
		sourceFileName
	]

	try:
		result = subprocess.run(command, capture_output=True, text=True, check=True)

	except subprocess.CalledProcessError as e:
		print(f"Can't compile [{sourceFileName}]: {e.stderr}")
		sys.exit(1)

	# get faust output and strip everything before class or static function definitions
	text = result.stdout.splitlines(keepends=True)
	found = False
	line = 0

	while not found:
		if text[line].startswith("class") or text[line].startswith("static"):
			found = True

		else:
			line += 1

	text = "".join(text[line:-4]) + "\n};\n"

	# patch generated code so it works for us (and looks a little better :-)
	text = text.replace("FAUSTFLOAT", "float")
	text = text.replace("RESTRICT ", "")
	text = text.replace("\t\n private:\n\t", "private:")
	text = text.replace("\t\n  private:\n", "private:")
	text = text.replace("\t\n  public:\n", "\npublic:")
	text = text.replace("\n};", "};")
	text = text.replace("void metadata(Meta* m)", "void metadata(Meta* m) override")
	text = text.replace("virtual int getNumInputs()", "int getNumInputs() override")
	text = text.replace("virtual int getNumOutputs()", "int getNumOutputs() override")
	text = text.replace("virtual void init(int sample_rate)", "void init(int sample_rate) override")
	text = text.replace("virtual int getSampleRate()", "int getSampleRate() override")
	text = text.replace("virtual void buildUserInterface(UI* ui_interface)", "void buildUserInterface(UI* ui_interface) override")
	text = text.replace("virtual void compute(int count, float** inputs, float** outputs)", "void compute(int count, float** inputs, float** outputs) override")
	text = text.replace("int sample_rate", "[[maybe_unused]] int sample_rate")

	# write code to target file
	with open(os.path.join("..", className + ".h"), "w") as output:
		output.write(header.replace("CLASS", className))
		output.write(text)
