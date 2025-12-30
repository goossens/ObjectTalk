//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "flanger";
declare name "Flanger";
declare category "Modulation";
declare license "MIT";

import("stdfaust.lib");

delay = hslider("[0]Delay[format: %.1fms]", 0.2, 0, 10, 0.1): si.smoo;
depth = hslider("[1]Depth[format: %.0f%%]", 50, 0, 100, 1): si.smoo;
feedback = hslider("[2]Feedback[format: %.0f%%]", 60, 0, 100, 1): si.smoo;

process = _ : pf.flanger_mono(512, delay * 51.2, depth / 100.0, feedback / 100.0, 0);
