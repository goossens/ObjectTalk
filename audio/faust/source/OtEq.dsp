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
	eqGroup(x) = hgroup("[0] Parametric EQ", x);

	lsGroup(x) = eqGroup(vgroup("[1]Low", x));
	LL = lsGroup(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40, 40, 0.1));
	FL = lsGroup(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 1, 1000, 1));

	pq1Group(x) = eqGroup(vgroup("[2]Peak1", x));
	LP1 = pq1Group(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40 , 40, 0.1));
	FP1 = pq1Group(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 20, 10000, 1)) : si.smooth(0.999);
	Q1 = pq1Group(hslider("[2]Q[style:knob]", 1, 0.5, 10, 0.1));

	pq2Group(x) = eqGroup(vgroup("[3]Peak2", x));
	LP2 = pq2Group(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40 , 40, 0.1));
	FP2 = pq2Group(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 20, 10000, 1)) : si.smooth(0.999);
	Q2 = pq2Group(hslider("[2]Q[style:knob]", 1, 0.5, 10, 0.1));

	pq3Group(x) = eqGroup(vgroup("[4]Peak3", x));
	LP3 = pq3Group(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40 , 40, 0.1));
	FP3 = pq3Group(hslider("[1]Freq[style:knob][format: %.0fhz]", 100, 20, 10000, 1)) : si.smooth(0.999);
	Q3 = pq3Group(hslider("[2]Q[style:knob]", 1, 0.5, 10, 0.1));

	hsGroup(x) = eqGroup(vgroup("[5]High", x));
	LH = hsGroup(hslider("[0]Level[style:knob][format: %.1fdB]", 0, -40, 40, 0.1));
	FH = hsGroup(hslider("[1]Freq[style:knob][format: %.0fhz]", 8000, 20, 10000, 1));
};
