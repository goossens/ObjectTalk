//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Graphic EQ";
declare category "Equalizer";
declare license "MIT";

import("stdfaust.lib");

process = fi.mth_octave_filterbank_default(1, 16000, N) : sum(i, N, (*(ba.db2linear(fader(N - i))))) with {
	N = 10;
	sliders(x) = hgroup("", x);
	fader(i) = sliders(vslider("Band%i[format: %.1fdB]", -10, -70, 10, 0.1)) : si.smoo;
};
