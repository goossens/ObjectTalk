//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "ADSR";
declare category "Envelope";
declare license "MIT";

import("stdfaust.lib");

process = fi.low_shelf(LL, FL) : fi.peak_eq_cq(LP1, FP1, Q1) : fi.peak_eq_cq(LP2, FP2, Q2) : fi.peak_eq_cq(LP3, FP3, Q3) : fi.high_shelf(LH, FH) with {
	eq_group(x) = hgroup("[0] Parametric EQ", x);

	ls_group(x) = eq_group(vgroup("[1]Low", x));
	LL = ls_group(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40, 40, 0.1));
	FL = ls_group(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 1, 1000, 1));

	pq1_group(x) = eq_group(vgroup("[2]Peak1", x));
	LP1 = pq1_group(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40 , 40, 0.1));
	FP1 = pq1_group(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 20, 10000, 1)) : si.smooth(0.999);
	Q1 = pq1_group(hslider("[2]Q[style:knob]", 1, 0.5, 10, 0.1));

	pq_group2(x) = eq_group(vgroup("[3]Peak2", x));
	LP2 = pq_group2(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40 , 40, 0.1));
	FP2 = pq_group2(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 20, 10000, 1)) : si.smooth(0.999);
	Q2 = pq_group2(hslider("[2]Q[style:knob]", 1, 0.5, 10, 0.1));

	pq_group3(x) = eq_group(vgroup("[4]Peak3", x));
	LP3 = pq_group3(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40 , 40, 0.1));
	FP3 = pq_group3(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 20, 10000, 1)) : si.smooth(0.999);
	Q3 = pq_group3(hslider("[2]Q[style:knob]", 1, 0.5, 10, 0.1));

	hs_group(x) = eq_group(vgroup("[5]High", x));
	LH = hs_group(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40, 40, 0.1));
	FH = hs_group(hslider("[1]Freq[style:knob][format: %.0fhz]", 8000, 20, 10000, 1));
};
