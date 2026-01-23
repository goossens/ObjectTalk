//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Spring Reverb";
declare category "Reverb";
declare license "MIT";

import("stdfaust.lib");

dwell = hslider("[0]Dwell", 0.2, 0, 1, 0.01): si.smoo;
tension = hslider("[1]Tension", 0, 0, 1, 0.01): si.smoo;
blend = hslider("[2]Blend", 0.5, 0, 1, 0.01): si.smoo;

process = re.springreverb(dwell, blend, 0.5, tension, 2);
