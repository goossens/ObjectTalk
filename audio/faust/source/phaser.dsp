//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "phaser";
declare name "Phaser";
declare category "Modulation";
declare license "MIT";

import("stdfaust.lib");

depth = hslider("Depth", 0.5, 0, 3, 0.01): si.smoo;
feedback = hslider("Feedback", 0, 0, 1, 0.01): si.smoo;
speed = hslider("Speed", 1, 0.001, 20, 0.01): si.smoo;
process = _ : pf.phaser2_mono(8, 0, 10000, 20, 1, 20000, speed, depth, feedback, 0);
