//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Tube Screamer";
declare category "Distortion";
declare description "Ibanez TS-9 simulation";

import("stdfaust.lib");

process = ts9 : lowpass : *(gain) with {
	R1 = 4700;
	R2 = 51000 + 500000 * hslider("[0]Drive[style:knob]", 0.5, 0, 1, 0.01);
	C = 0.047 * 1e-6;
	a1 = (R1 + R2) * C * 2 * ma.SR;
	a2 = R1 * C * 2 * ma.SR;
	B0 = (1 + a1) / (1 + a2);
	B1 = (1 - a1) / (1 + a2);
	A1 = (1 - a2) / (1 + a2);
	X2 = fi.tf1(B0, B1, A1);
	ts9nonlin = ffunction(float OtTs9(float), "OtTs9.h", "");
	ts9 = _ <: _ - ts9nonlin(X2 - _) :> _;
	fc = hslider("[1]Tone[style:knob]", 400, 100, 1000, 1.03);
	lowpass = fi.lowpass(1, fc);
	gain = hslider("[2]Level[style:knob][format: %.1fdB]", -16, -20, 4, 0.1) : ba.db2linear : si.smooth(0.999);
};
