//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

import("stdfaust.lib");
import("OtTube.lib");

amp = fi.iir((b0/a0,b1/a0,b2/a0),(a1/a0,a2/a0)) : powerAmpTube(TB_SINGLE_ENDED_6V6, TB_SINGLE_ENDED_6V6) with {
	fs = float(ma.SR);
	b0 = fs * (2.46383752600217e-9 * fs + 1.18536469845222e-7);
	b1 = -4.92767505200435e-9 * pow(fs, 2);
	b2 = fs * (2.46383752600217e-9 * fs - 1.18536469845222e-7);
	a0 = fs * (4.31429171140134e-10 * fs + 1.24411557886099e-7) + 1.54349145985544e-5;
	a1 = -8.62858342280268e-10 * pow(fs, 2) + 3.08698291971088e-5;
	a2 = fs * (4.31429171140134e-10 * fs - 1.24411557886099e-7) + 1.54349145985544e-5;
};

feedbackCircuit = presenceFilter : *(feedback) with {
	presence = hslider("[0]Presence[style:knob]", 0, -15, 15, 0.1);
	presenceFilter = wa.peaking2(2000, presence, 1, 0) : wa.peaking2(4000, presence, 1, 0);
	feedback = hslider("[1]Feedback[label:Feedback][style:knob]", 0, -0.4, 1, 0.01) : si.smoo;
};

process = hgroup("PowerAmp", (+ : amp : fi.dcblocker) ~ feedbackCircuit : *(hslider("[2]Volume[style:knob]", 1.0, 0.0, 2.0, 0.1)));