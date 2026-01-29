#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

import glob, json, os, pathlib, re, subprocess, sys

# patterns in original source code
buildUserInterfacePattern = r"\n\tvoid buildUserInterface(.*?)\}\n"

# process UI and generate UI and parameter code
windowFlags = "ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY"
sliderLength = "100.0f"
sliderWidth = "20.0f"

class UI:
	def __init__(self, item, className):
		self.ui = []
		self.size = []
		self.parameters = []
		self.variables = []
		self.level = 1
		self.child = 1

		self.processItem(item)

	def processItem(self, item):
		match item["type"]:
			case "vgroup":
				return self.group(item, True)

			case "hgroup":
				return self.group(item, False)

			case "vslider":
				return self.slider(item, True)

			case "hslider":
				return self.slider(item, False)

			case _:
				return "0.0f", "0.0f"

	def group(self, item, vertical):
		width = f"width{self.level}"
		height = f"height{self.level}"
		self.variables.append(f"\tfloat {width};")
		self.variables.append(f"\tfloat {height};")
		label = self.generateChildLabel(item["label"])

		if self.level == 1 or item["label"] == "0x00":
			self.ui.append(f"\t\tImGui::BeginChild(\"{label}\", ImVec2(), {windowFlags});")

		else:
			self.ui.append(f"\t\tImGui::BeginChild(\"{label}\", ImVec2(), {windowFlags} | ImGuiChildFlags_Border);")
			self.ui.append(f"\t\tOtUi::Header(\"{item["label"]}\");")
			self.size.append(f"\t\t{width} += border * 2.0f + padding.x * 2.0f;")
			self.size.append(f"\t\t{height} += frame + border * 2.0f + padding.y * 2.0f;")

		self.level += 1
		start = len(self.ui)

		for child in item["items"]:
			if vertical:
				w, h = self.processItem(child)
				self.size.append(f"\t\t{width} = std::max({width}, {w});")
				self.size.append(f"\t\t{height} += {h};")

			else:
				if len(self.ui) != start:
					self.ui.append("\t\tImGui::SameLine();")
					self.size.append(f"\t\t{width} += spacing;")

				w, h = self.processItem(child)
				self.size.append(f"\t\t{width} += {w};")
				self.size.append(f"\t\t{height} = std::max({height}, {h});")

		self.level -= 1
		self.ui.append("\t\tImGui::EndChild();")
		return width, height

	def slider(self, item, vertical):
		meta = self.metaToDict(item.get("meta", []))
		style = meta.get("style", "")
		label = item.get("label", "")
		varname = item.get("varname", "")
		address = self.fixAddress(item.get("address", ""))
		parname = address[0].lower() + address[1:]
		initValue = self.numberToString(item.get("init", 0))
		minValue = self.numberToString(item.get("min", 0))
		maxValue = self.numberToString(item.get("max", 0))
		stepValue = self.numberToString(item.get("step", 0))
		format = meta.get("format", self.formatFromStepValue(stepValue))

		self.parameters.append({
			"label": label,
			"varname": varname,
			"parname": parname,
			"address": address,
			"init": initValue,
			"min": minValue,
			"max": maxValue,
			"step": stepValue})

		if style == "knob":
			self.ui.append(f"\t\tchanged |= OtUi::knob(\"{label}\", &{varname}, {minValue}, {maxValue}, \"{format}\");")
			return "knobWidth", "knobHeight"

		elif vertical:
			self.ui.append(f"\t\tchanged |= ImGui::VSliderFloat(\"{label}\", ImVec2({sliderWidth}, {sliderLength}), &{varname}, {minValue}, {maxValue}, \"{format}\");")
			return sliderWidth, sliderLength

		else:
			self.ui.append(f"\t\tImGui::SetNextItemWidth({sliderLength});")
			self.ui.append(f"\t\tchanged |= ImGui::SliderFloat(\"{label}\", &{varname}, {minValue}, {maxValue}, \"{format}\");")
			return sliderLength, sliderWidth

	def getSize(self):
		text = "\n".join(self.size)
		intro = ""
		if "knobWidth" in text: intro += "\t\tauto knobWidth = OtUi::knobWidth();\n"
		if "knobHeight" in text: intro += "\t\tauto knobHeight = OtUi::knobHeight();\n"
		if "spacing" in text: intro += "\t\tauto spacing = ImGui::GetStyle().ItemSpacing.x;\n"
		if "frame" in text: intro += "\t\tauto frame = ImGui::GetFrameHeightWithSpacing();\n"
		if "border" in text: intro += "\t\tauto border = ImGui::GetStyle().ChildBorderSize;\n"
		if "padding" in text: intro += "\t\tauto padding = ImGui::GetStyle().WindowPadding;\n"
		return intro + text

	def getRenderUI(self):
		return "\n".join(self.ui)

	def getParametersStruct(self):
		if self.parameters:
			return "\n".join([f"\t\tfloat {parameter["parname"]};" for parameter in self.parameters])

		else:
			return "\t\tfloat dummy;"

	def getSetParameters(self):
		return "\n".join([f"\t\t{parameter["varname"]} = parameters.{parameter["parname"]};" for parameter in self.parameters])

	def getGetParameters(self):
		return "\n".join([f"\t\tparameters.{parameter["parname"]} = {parameter["varname"]};" for parameter in self.parameters])

	def getIterateParameters(self):
		return "\n".join([f"\t\tcallback(\"{parameter["parname"]}\", &{parameter["varname"]}, {parameter["init"]});" for parameter in self.parameters])

	def getSetters(self):
		return "\n".join([f"\tinline void set{parameter["address"]}(float value) {{ {parameter["varname"]} = value; }}" for parameter in self.parameters])

	def getGetters(self):
		return "\n".join([f"\tinline float get{parameter["address"]}() {{ return {parameter["varname"]}; }}" for parameter in self.parameters])

	def getVariables(self):
		return "\n".join(self.variables)

	def metaToDict(self, meta):
		result = {}

		for item in meta:
			result |= item

		return result

	def numberToString(self, number):
		result = str(number)
		result += "" if "." in result else ".0"
		result += "f"
		return result

	def generateChildLabel(self, label):
		if label == "0x00":
			label = f"Label{self.child}"
			self.child += 1

		return label

	def fixAddress(self, address):
		if address[0] == "/":
			address = address[1:]
			address = address[address.find("/"):]
			address = address.replace("/", "")

		address = address.replace("0x00", "")
		return address

	def formatFromStepValue(self, stepValue):
		stepValue = float(stepValue[0:-1])

		if stepValue < 0.1:
			return "%.2f"

		elif stepValue < 1.0:
			return "%.1f"

		else:
			return "%.0f"

# process DSP file and generate code
def processDspFile(sourceFileName):
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

	# process UI
	ui = UI(metadata["ui"][0], className)

	# patch generated code so it works for us (and looks a little better :-)
	headerText = header.replace("CLASS", className)

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
	text = re.sub(buildUserInterfacePattern, "", text, flags=re.DOTALL)

	footerText = footer \
		.replace("SIZE", ui.getSize()) \
		.replace("RENDERUI", ui.getRenderUI()) \
		.replace("PARAMETERSSTRUCT", ui.getParametersStruct()) \
		.replace("SETPARAMETERS", ui.getSetParameters()) \
		.replace("GETPARAMETERS", ui.getGetParameters()) \
		.replace("ITERATEPARAMETERS", ui.getIterateParameters()) \
		.replace("SETTERS", ui.getSetters()) \
		.replace("GETTERS", ui.getGetters()) \
		.replace("VARIABLES", ui.getVariables())

	code = headerText + text + footerText

	# write code to target file
	with open(os.path.join("..", className + ".h"), "w") as output:
		output.write(code)

#
#	Main
#

# switch directory to script location
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# read standard header and footer
with open("OtFaustHeader.h", "r") as file:
	header = file.read()

with open("OtFaustFooter.h", "r") as file:
	footer = file.read()

# process all DSP files (that are new or changed) and generate code
for sourceFileName in glob.iglob("*.dsp"):
	targetFileName = os.path.join("..", (pathlib.Path(sourceFileName).stem) + ".h")

	if not os.path.exists(targetFileName) or os.path.getmtime(sourceFileName) > os.path.getmtime(targetFileName):
		processDspFile(sourceFileName)
