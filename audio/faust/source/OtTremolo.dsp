//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Tremolo";
declare category "Modulation";
declare license "MIT";

import("stdfaust.lib");

process = hgroup("Tremolo", *(lfo * depth + 1 - depth) with {
	speed = vslider("[0]Speed[style:knob][format: %.01fhz]", 8, 0.1, 50, 0.01) : si.smoo;
	depthPercent = vslider("[1]Depth[style:knob][format: %.0f%%]", 50, 0, 100, 1) : si.smoo;
	depth = depthPercent / 100;
	lfo = (os.oscs(speed) + 1) * 0.5 : max(0);
});
