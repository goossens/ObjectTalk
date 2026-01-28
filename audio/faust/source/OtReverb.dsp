//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Spring Reverb";
declare category "Reverb";
declare license "MIT";

import("stdfaust.lib");

dwell = vslider("[0]Dwell[style:knob]", 0.2, 0, 1, 0.01): si.smoo;
tension = vslider("[1]Tension[style:knob]", 0, 0, 1, 0.01): si.smoo;
blend = vslider("[2]Blend[style:knob]", 0.5, 0, 1, 0.01): si.smoo;

process = hgroup("Reverb", re.springreverb(dwell, blend, 0.5, tension, 2));
