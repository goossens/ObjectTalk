#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

import os, subprocess, sys

units = [
	["chorus", "OtChorus"],
	["flanger", "OtFlanger"],
	["phaser", "OtPhaser"],
	["reverb", "OtReverb"],
	# ["champ", "OtChamp"],
	# ["compressor", "OtCompressor"],
	# ["delay", "OtDelay"],
	# ["echo", "OtEcho"],
	# ["epiphone", "OtEpiphone"],
	# ["fuzzface", "OtFuzzFace"],
	# ["graphiceq", "OtGraphicsEq"],
	# ["gxamp", "OtGxAmp1"],
	# ["gxamp2", "OtGxAmp2"],
	# ["gxamp3", "OtGxAmp3"],
	# ["gxamp4", "OtGxAmp4"],
	# ["gxamp5", "OtGxAmp5"],
	# ["gxamp6", "OtGxAmp6"],
	# ["gxamp7", "OtGxAmp7"],
	# ["gxamp8", "OtGxAmp8"],
	# ["gxamp9", "OtGxAmp9"],
	# ["gxamp10", "OtGxAmp10"],
	# ["gxamp11", "OtGxAmp11"],
	# ["gxamp12", "OtGxAmp12"],
	# ["gxamp13", "OtGxAmp13"],
	# ["gxamp14", "OtGxAmp14"],
	# ["gxamp15", "OtGxAmp15"],
	# ["gxamp16", "OtGxAmp16"],
	# ["gxamp17", "OtGxAmp17"],
	# ["gxamp18", "OtGxAmp18"],
	# ["gxdistortion", "OtDistortion"],
	# ["orangedarkterror", "OtOrangeDarkTerror"],
	# ["overdrive", "OtOverdrive"],
	# ["plexiel34", "OtPlexieEl34"],
	# ["princeton", "OtPrinceton"],
	# ["supersonic", "OtSupersonic"],
	# ["tonestack_ac15", "OtToneStackAc15"],
	# ["tonestack_ac30", "OtToneStackAc30"],
	# ["tonestack_ampeg_rev", "OtToneStackAmpegRev"],
	# ["tonestack_ampeg", "OtToneStackAmpeg"],
	# ["tonestack_bassman", "OtToneStackBaseman"],
	# ["tonestack_bogner", "OtToneStackBogner"],
	# ["tonestack_crunch", "OtToneStackCrunch"],
	# ["tonestack_engl", "OtToneStackEngle"],
	# ["tonestack_fender_blues", "OtToneStackFenderBlues"],
	# ["tonestack_fender_default", "OtToneStackFenderDefault"],
	# ["tonestack_fender_deville", "OtToneStackFenderDeville"],
	# ["tonestack_gibsen", "OtToneStackGibson"],
	# ["tonestack_groove", "OtToneStackGroove"],
	# ["tonestack_ibanez", "OtToneStackIbanez"],
	# ["tonestack_jcm2000", "OtToneStackJcm2000"],
	# ["tonestack_jcm800", "OtToneStackJcm800"],
	# ["tonestack_jtm45", "OtToneStackJtm45"],
	# ["tonestack_m2199", "OtToneStackM2199"],
	# ["tonestack_mesa", "OtToneStackMesa"],
	# ["tonestack_mlead", "OtToneStackMlead"],
	# ["tonestack_peavey", "OtToneStackPeavey"],
	# ["tonestack_princeton", "OtToneStackPrinceton"],
	# ["tonestack_roland", "OtToneStackRowland"],
	# ["tonestack_soldano", "OtToneStackSoldano"],
	# ["tonestack_sovtek", "OtToneStackSovtek"],
	# ["tonestack_twin", "OtToneStackTwin"]
]

# switch directory to script's parent directory
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# read header file
with open("source/header.h", "r") as file:
	header = file.read()

# process all DSP files and generate c++ code
for [src, dst] in units:
	command = [
		"faust",
		# "-a", os.path.join("source", "architecture.tmpl"),
		"--language", "cpp",
		"--double-precision-floats",
		"--class-name", dst,
		"--super-class-name", "OtFaust",
		os.path.join("source", src + ".dsp")
	]

	try:
		result = subprocess.run(command, capture_output=True, text=True, check=True)

	except subprocess.CalledProcessError as e:
		print(f"Can't compile [{src}]: {e.stderr}")
		sys.exit(1)

	text = result.stdout.splitlines(keepends=True)

	line = 0
	found = False

	while not found:
		if text[line].startswith("class") or text[line].startswith("static"):
			found = True
		else:
			line += 1

	text = "".join(text[line:-4])
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

	with open(dst + ".h", "w") as output:
		output.write(header.replace("CLASS", dst))
		output.write(text)
		output.write("};\n")
