//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Phaser";
declare category "Modulation";
declare license "MIT";

import("stdfaust.lib");

speed = vslider("[0]Speed[style:knob][format: %.01fhz]", 1, 0.001, 20, 0.01) : si.smoo;
depth = vslider("[1]Depth[style:knob][format: %.0f%%]", 50, 0, 100, 1) : si.smoo;
feedback = vslider("[3]Feedbck[style:knob]", 0, -1, 1, 0.01) : si.smoo;

process = hgroup("Phaser", pf.phaser2_mono(8, 0, 10000, 20, 1, 20000, speed, depth / 100.0, feedback, 0));
