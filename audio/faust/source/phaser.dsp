//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "phaser";
declare name "Phaser";
declare category "Modulation";
declare license "MIT";

import("stdfaust.lib");

speed = hslider("[0]Speed[format: %.01fhz]", 1, 0.001, 20, 0.01): si.smoo;
depth = hslider("[1]Depth[format: %.0f%%]", 50, 0, 100, 1): si.smoo;
feedback = hslider("[3]Feedback", 0, -1, 1, 0.01): si.smoo;

process = _ : pf.phaser2_mono(8, 0, 10000, 20, 1, 20000, speed, depth, feedback, 0);
