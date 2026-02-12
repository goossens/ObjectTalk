//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Ladder";
declare category "Filter";
declare license "MIT";

import("stdfaust.lib");

filterGroup(x) = hgroup("[0]Ladder", x);
cutoff = filterGroup(vslider("[2]Freq[style:knob][format: %.0fhz]", 800, 80, 8000, 0.1)) : si.smoo;
resonance = filterGroup(vslider("[3]Res[style:knob]", 0.5, 0, 1, 0.01));

process(signal, mod1, mod2, mod3) = signal : ve.lowpassLadder4(resonance * 4, freq) with {
	freq = cutoff * (2 ^ (mod1 + mod2 + mod3));
};
