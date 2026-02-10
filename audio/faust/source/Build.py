#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

import glob, json, os, pathlib, re, subprocess, sys

# process UI and generate UI and parameter code
class UI:
	# constructor
	def __init__(self, item, className):
		self.ui = []
		self.size = []
		self.editors = []
		self.parameters = []
		self.level = 1
		self.child = 1

		w, h = self.processItem(item)
		self.size.append(f"\t\twidth = {w};")
		self.size.append(f"\t\theight = {h};")

	# process an item in the UI tree
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

			case "checkbox":
				return self.checkbox(item)

			case "button":
				return self.button(item)

			case _:
				return "0.0f", "0.0f"

	# process a group of items (either with vertical or horizontal layout)
	def group(self, item, vertical):
		label = item["label"]

		if label == "0x00":
			label = f"Label{self.child}"

		width = f"width{self.child}"
		height = f"height{self.child}"
		self.child += 1

		self.size.append(f"\t\tfloat {width} = 0.0f;")
		self.size.append(f"\t\tfloat {height} = 0.0f;")
		self.ui.append(f"\t\tImGui::BeginGroup();")
		self.ui.append(f"\t\tImGui::PushID(\"{label}\");")

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
					self.size.append(f"\t\t{width} += spacing.x;")

				w, h = self.processItem(child)
				self.size.append(f"\t\t{width} += {w};")
				self.size.append(f"\t\t{height} = std::max({height}, {h});")

		self.level -= 1
		self.ui.append("\t\tImGui::PopID();")
		self.ui.append("\t\tImGui::EndGroup();")
		return width, height

	# process an a slider (vertical or horizontally) or a knob
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
		log = "true" if meta.get("scale", "") == "log" else "false"
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

		sliderLength = "100.0f"
		sliderWidth = "20.0f"

		if style == "knob":
			self.editors.append(f"\tinline bool edit{address}() {{ return OtUi::knob(\"{label}\", &{varname}, {minValue}, {maxValue}, \"{format}\", {log}); }}")
			self.ui.append(f"\t\tchanged |= edit{address}();")
			return "knobWidth", "knobHeight"

		elif vertical:
			self.editors.append(f"\tinline bool edit{address}() {{ return ImGui::VSliderFloat(\"{label}\", ImVec2({sliderWidth}, {sliderLength}), &{varname}, {minValue}, {maxValue}, \"{format}\"); }}")
			self.ui.append(f"\t\tchanged |= edit{address}();")
			return sliderWidth, sliderLength

		else:
			self.editors.append(f"\tinline bool edit{address}() {{ ImGui::SetNextItemWidth({sliderLength}); return ImGui::SliderFloat(\"{label}\", &{varname}, {minValue}, {maxValue}, \"{format}\"); }}")
			self.ui.append(f"\t\tchanged |= edit{address}();")
			return sliderLength, sliderWidth

	# process a checkbox with an optional left and right label
	def checkbox(self, item):
		label = item.get("label", "")
		varname = item.get("varname", "")
		address = self.fixAddress(item.get("address", ""))
		parname = address[0].lower() + address[1:]

		self.parameters.append({
			"label": label,
			"varname": varname,
			"parname": parname,
			"address": address,
			"init": "0.0f",
			"min": "0.0f",
			"max": "1.0f",
			"step": "1.0f"})

		meta = self.metaToDict(item.get("meta", []))
		labels = meta.get("labels", "Off|On")
		offLabel, onLabel = labels.split("|")

		if len(offLabel):
			self.ui.append(f"\t\tImGui::AlignTextToFramePadding();")
			self.ui.append(f"\t\tImGui::TextUnformatted(\"{offLabel}\");")
			self.ui.append(f"\t\tImGui::SameLine();")

		self.ui.append(f"\t\tchanged |= OtAudioUi::toggleButton(\"##{label}\", &{varname});")

		if len(onLabel):
			self.ui.append(f"\t\tImGui::SameLine();")
			self.ui.append(f"\t\tImGui::AlignTextToFramePadding();")
			self.ui.append(f"\t\tImGui::TextUnformatted(\"{onLabel}\");")

		checkBoxWidth = ""

		if len(offLabel):
			checkBoxWidth += f"ImGui::CalcTextSize(\"{offLabel}\").x + spacing.x + "

		checkBoxWidth += "OtUi::toggleButtonWidth()"

		if len(onLabel):
			checkBoxWidth += f" + ImGui::CalcTextSize(\"{onLabel}\").x + spacing.x);"

		return checkBoxWidth, "frame"

	# process a push button
	def button(self, item):
		label = item.get("label", "")
		varname = item.get("varname", "")
		address = self.fixAddress(item.get("address", ""))
		parname = address[0].lower() + address[1:]

		self.parameters.append({
			"label": label,
			"varname": varname,
			"parname": parname,
			"address": address,
			"init": "0.0f",
			"min": "0.0f",
			"max": "1.0f",
			"step": "1.0f"})

		self.editors.append(f"\tinline bool edit{address}() {{ ImGui::Button(\"{label}\"); return ImGui::IsItemActive() ? 1.0f : 0.0f; }}")
		self.ui.append(f"\t\tchanged |= edit{address}();")
		return f"ImGui::CalcTextSize(\"{label}\").x + ImGui::GetStyle().FramePadding.x * 2.0f", "frame"

	# support functions to concatenate chucks of code
	def getSize(self):
		text = "\n".join(self.size)
		intro = ""
		if "knobWidth" in text: intro += "\t\tauto knobWidth = OtUi::knobWidth();\n"
		if "knobHeight" in text: intro += "\t\tauto knobHeight = OtUi::knobHeight();\n"
		if "spacing" in text: intro += "\t\tauto spacing = ImGui::GetStyle().ItemSpacing;\n"
		if "frame" in text: intro += "\t\tauto frame = ImGui::GetFrameHeightWithSpacing();\n"
		return intro + text

	def getRenderUI(self):
		return "\n".join(self.ui)

	def getStructParameters(self):
		if self.parameters:
			return "\n".join([f"\t\tfloat {parameter["parname"]} = {parameter["init"]};" for parameter in self.parameters])

		else:
			return "\t\tfloat dummy;"

	def getSetParameters(self):
		return "\n".join([f"\t\t{parameter["varname"]} = parameters.{parameter["parname"]};" for parameter in self.parameters])

	def getGetParameters(self):
		return "\n".join([f"\t\tparameters.{parameter["parname"]} = {parameter["varname"]};" for parameter in self.parameters])

	def getIterateParameters(self):
		return "\n".join([f"\t\tcallback(\"{parameter["parname"]}\", &{parameter["varname"]}, {parameter["init"]});" for parameter in self.parameters])

	def getEditors(self):
		return "\n".join(self.editors)

	def getSetters(self):
		return "\n".join([f"\tinline void set{parameter["address"]}(float value) {{ {parameter["varname"]} = value; }}" for parameter in self.parameters])

	def getGetters(self):
		return "\n".join([f"\tinline float get{parameter["address"]}() {{ return {parameter["varname"]}; }}" for parameter in self.parameters])

	# support functions to convert
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
def processDspFile(sourceFileName, targetFileName):
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

	# process UI information from JSON metadata
	ui = UI(metadata["ui"][0], className)

	# patch generated code so it works for us (and looks a little better :-)
	headerText = header.replace("CLASS", className)

	text = text.replace("FAUSTFLOAT", "float")
	text = text.replace("RESTRICT ", "")
	text = text.replace("\t\n private:\n\t", "protected:")
	text = text.replace("\t\n  private:\n", "protected:")
	text = text.replace("\t\n  public:\n", "\npublic:")
	text = text.replace("void metadata", "inline void metadata")
	text = text.replace("virtual int", "inline int")
	text = text.replace("virtual void", "inline void")
	text = text.replace("inline void compute(int count, float** inputs, float** outputs)", "inline void compute(int count, [[maybe_unused]] float** inputs, float** outputs)")
	text = text.replace("int sample_rate", "[[maybe_unused]] int sample_rate")
	text = re.sub(rf"\t{className}\(\) \{{\n\t\}}\n", f"\t{className}() {{\n\t\tinit(OtAudioSettings::sampleRate);\n\t}}\n", text, flags=re.DOTALL)
	text = re.sub(rf"\n\t{className}\(const {className}\&\) = default;\n", "", text, flags=re.DOTALL)
	text = re.sub(rf"\n\tvirtual \~{className}\(\) = default;\n", "", text, flags=re.DOTALL)
	text = re.sub(rf"\n\t{className}\& operator=\(const {className}\&\) = default;\n", "", text, flags=re.DOTALL)
	text = re.sub(rf"\n\tvirtual {className}\* clone(.*?)\}}\n", "", text, flags=re.DOTALL)
	text = re.sub(r"\n\tinline void buildUserInterface(.*?)\}\n", "", text, flags=re.DOTALL)

	footerText = footer \
		.replace("SIZE", ui.getSize()) \
		.replace("RENDERUI", ui.getRenderUI()) \
		.replace("STRUCTPARAMETERS", ui.getStructParameters()) \
		.replace("SETPARAMETERS", ui.getSetParameters()) \
		.replace("GETPARAMETERS", ui.getGetParameters()) \
		.replace("ITERATEPARAMETERS", ui.getIterateParameters()) \
		.replace("EDITORS", ui.getEditors()) \
		.replace("SETTERS", ui.getSetters()) \
		.replace("GETTERS", ui.getGetters())

	code = headerText + text + footerText

	# write code to target file
	with open(targetFileName, "w") as output:
		output.write(code)

#
#	Main
#

# switch directory to script location
scriptPath = os.path.abspath(__file__)
os.chdir(os.path.dirname(scriptPath))

# read standard header and footer
with open("OtFaustHeader.h", "r") as file:
	header = file.read()

with open("OtFaustFooter.h", "r") as file:
	footer = file.read()

# process all DSP files (that are new or changed) and generate code
for sourceFileName in glob.iglob("*.dsp"):
	targetFileName = os.path.join("..", (pathlib.Path(sourceFileName).stem) + ".h")
	exists = os.path.exists(targetFileName)
	sourceNewer = exists and os.path.getmtime(sourceFileName) > os.path.getmtime(targetFileName)
	scriptNewer = exists and os.path.getmtime(scriptPath) > os.path.getmtime(targetFileName)

	if not exists or sourceNewer or scriptNewer:
		print(f"Compiling [{sourceFileName}]...")
		processDspFile(sourceFileName, targetFileName)
