//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "compressor";
declare name "Compressor";
declare category "Compressor";
declare license "MIT";

import("stdfaust.lib");

ratio = hslider("[0]Ratio", 4, 1, 20, 0.1): si.smoo;
threshold = hslider("[1]Thresh[format: %.0fdB]", -10, -50, 0, 1): si.smoo;
attack = hslider("[2]Attack[format: %.0fms]", 10, 0, 200, 1): si.smoo;
release = hslider("[3]Release[format: %.0fms]", 200, 5, 1000, 1): si.smoo;

process = _ : co.compressor_mono(ratio, threshold, attack / 1000.0, release / 1000.0);
