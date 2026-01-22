#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

import glob, json, os, pathlib, subprocess, sys

# recursively find specified key in list or dictionary
def findKey(data, key):
	if isinstance(data, list):
		for item in data:
			yield from findKey(item, key)

	if isinstance(data, dict):
		if key in data.keys():
			yield data[key]

		for item in data.values():
			yield from findKey(item, key)

# switch directory to script location
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# read standard header
with open("OtFaustHeader.h", "r") as file:
	header = file.read()

# process all DSP files and generate code
for sourceFileName in glob.iglob("*.dsp"):
	className = pathlib.Path(sourceFileName).stem
	jsonName = sourceFileName + ".json"

	# run faust to generate C++ code
	command = [
		"faust",
		"--language", "cpp",
		"--double-precision-floats",
		"--class-name", className,
		"--super-class-name", "OtFaust",
		"--json",
		sourceFileName
	]

	try:
		result = subprocess.run(command, capture_output=True, text=True, check=True)

	except subprocess.CalledProcessError as e:
		print(f"Can't compile [{sourceFileName}]: {e.stderr}")
		sys.exit(1)

	# get faust output
	text = result.stdout.splitlines(keepends=True)

	# get metadata from json file
	with open(jsonName, "r") as file:
		metadata = json.load(file)

	os.remove(jsonName)

	# strip everything before class or static function definitions
	found = False
	line = 0

	while not found:
		if text[line].startswith("class") or text[line].startswith("static"):
			found = True

		else:
			line += 1

	text = "".join(text[line:-4])

	# convert metadata to getters, setters and parameter structures
	variableDefinition = []
	variableLoad = []
	variableSave = []
	getters = []
	setters = []

	for variables in findKey(metadata, "items"):
		for variable in variables:
			if "varname" in variable:
				name = variable["varname"]
				label = variable["label"]
				labelLC = label.lower()
				variableDefinition.append(f"\t\tfloat {labelLC};")
				variableLoad.append(f"\t\t{name} = parameters.{labelLC};")
				variableSave.append(f"\t\tparameters.{labelLC} = {name};")
				getters.append(f"\tinline float get{label}() {{ return {name}; }}")
				setters.append(f"\tinline void set{label}(float value) {{ {name} = value; }}")

	if variableDefinition:
		extraDefinitions = f"\n\tstruct Parameters {{\n{"\n".join(variableDefinition)}\n\t}};\n"
		extraDefinitions += f"\n\tinline void setParameters(Parameters& parameters) {{\n{"\n".join(variableLoad)}\n\t}}\n"
		extraDefinitions += f"\n\tinline void getParameters(Parameters& parameters) {{\n{"\n".join(variableSave)}\n\t}}\n"
		extraDefinitions += f"\n{"\n".join(setters)}\n\n{"\n".join(getters)}\n"

	else:
		extraDefinitions = ""

	# patch generated code so it works for us (and looks a little better :-)
	text = text.replace("FAUSTFLOAT", "float")
	text = text.replace("RESTRICT ", "")
	text = text.replace("\t\n private:\n\t", "protected:")
	text = text.replace("\t\n  private:\n", "protected:")
	text = text.replace("\t\n  public:\n", "\npublic:")
	text = text.replace("void metadata(Meta* m)", "void metadata(Meta* m) override")
	text = text.replace("virtual int getNumInputs()", "int getNumInputs() override")
	text = text.replace("virtual int getNumOutputs()", "int getNumOutputs() override")
	text = text.replace("virtual void init(int sample_rate)", "void init(int sample_rate) override")
	text = text.replace("virtual int getSampleRate()", "int getSampleRate() override")
	text = text.replace("virtual void buildUserInterface(UI* ui_interface)", "void buildUserInterface(UI* ui_interface) override")
	text = text.replace("virtual void compute(int count, float** inputs, float** outputs)", "void compute(int count, [[maybe_unused]] float** inputs, float** outputs) override")
	text = text.replace("int sample_rate", "[[maybe_unused]] int sample_rate")
	text += extraDefinitions + "};\n"

	# write code to target file
	with open(os.path.join("..", className + ".h"), "w") as output:
		output.write(header.replace("CLASS", className))
		output.write(text)
