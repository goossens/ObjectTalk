//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "springreverb";
declare name "Spring Reverb";
declare category "Reverb";
declare license "MIT";

import("stdfaust.lib");

dwell = hslider("Dwell", 0.2, 0, 1, 0.01): si.smoo;
blend = hslider("Blend", 0.5, 0, 1, 0.01): si.smoo;
tension = hslider("Tension", 0, 0, 1, 0.01): si.smoo;

process = _ : re.springreverb(dwell, blend, 0.5, tension, 3);
