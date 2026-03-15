//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

import("stdfaust.lib");
import("OtTube.lib");

amp = fi.iir((b0/a0,b1/a0,b2/a0),(a1/a0,a2/a0)) : powerAmpTube(TB_SINGLE_ENDED_EL84_NEG, TB_SINGLE_ENDED_EL84_POS) with {
	fs = float(ma.SR);
	b0 = fs * (5.16589926047446e-10 * fs + 5.33527722168907e-8);
	b1 = -1.03317985209489e-9*pow(fs, 2);
	b2 = fs * (5.16589926047446e-10 * fs - 5.33527722168907e-8);
	a0 = fs * (4.33069857761234e-10 * fs + 4.59724862995143e-8) + 3.68375740341601e-7;
	a1 = -8.66139715522468e-10*pow(fs, 2) + 7.36751480683202e-7;
	a2 = fs * (4.33069857761234e-10 * fs - 4.59724862995143e-8) + 3.68375740341601e-7;
};

feedbackCircuit = presenceFilter : *(feedback) with {
	presence = hslider("[0]Presence[style:knob]", 0, -15, 15, 0.1);
	presenceFilter = wa.peaking2(2000, presence, 1, 0) : wa.peaking2(4000, presence, 1, 0);
	feedback = hslider("[1]Feedback[label:Feedback][style:knob]", 0, -0.4, 1, 0.01) : si.smoo;
};

process = hgroup("PowerAmp", (+ : amp : fi.dcblocker) ~ feedbackCircuit : *(hslider("[2]Volume[style:knob]", 1.0, 0.0, 2.0, 0.1)));