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

delay = hslider("Delay", 0.2, 0, 1, 0.01): si.smoo;
depth = hslider("Depth", 0.5, 0, 3, 0.01): si.smoo;
feedback = hslider("Feedback", 0, 0, 1, 0.01): si.smoo;

process = _ : pf.flanger_mono(4096, delay * 4096, depth, feedback, 0);
