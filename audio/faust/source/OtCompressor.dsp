//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Compressor";
declare category "Compressor";
declare license "MIT";

import("stdfaust.lib");

ratio = vslider("[0]Ratio[style:knob]", 4, 1, 20, 0.1) : si.smoo;
threshold = vslider("[1]Thresh[style:knob][format: %.0fdB]", -10, -50, 0, 1) : si.smoo;
attack = vslider("[2]Attack[style:knob][format: %.0fms]", 10, 0, 200, 1) : si.smoo;
release = vslider("[3]Release[style:knob][format: %.0fms]", 200, 5, 1000, 1) : si.smoo;

process = hgroup("Compressor", co.compressor_mono(ratio, threshold, attack / 1000.0, release / 1000.0));
