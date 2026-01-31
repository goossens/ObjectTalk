//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "ADSR";
declare category "Envelope";
declare license "MIT";

import("stdfaust.lib");

process = fi.low_shelf(LL, FL) : fi.peak_eq(LP, FP, BP) : fi.high_shelf(LH, FH)
with{
	eq_group(x) = hgroup("[0] Parametric EQ", x);

	ls_group(x) = eq_group(vgroup("[1]Low", x));
	LL = ls_group(hslider("[0]Level[style:knob]", 0, -40, 40, 0.1));
	FL = ls_group(hslider("[1]Freq[style:knob]", 200, 1, 5000, 1));

	pq_group(x) = eq_group(vgroup("[2]Notch", x));
	LP = pq_group(hslider("[0]Level[style:knob]", 0, -40 , 40, 0.1));
	FP = pq_group(hslider("[1]Freq[style:knob]", 1000, 20, 10000, 1)) : si.smooth(0.999);
	Q = pq_group(hslider("[2]Q[style:knob]", 40, 1, 1000, 0.1));
	BP = FP / Q;

	hs_group(x) = eq_group(vgroup("[3]High", x));
	LH = hs_group(hslider("[0]Level[style:knob]", 0, -40, 40, 0.1));
	FH = hs_group(hslider("[1]Freq[style:knob]", 8000, 20, 10000, 1));
};
