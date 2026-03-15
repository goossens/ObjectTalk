//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

import("stdfaust.lib");
import("OtTube.lib");

amp = fi.iir((b0/a0,b1/a0,b2/a0,b3/a0,b4/a0),(a1/a0,a2/a0,a3/a0,a4/a0)) : powerAmpTube(TB_PUSH_PULL_EL84_NEG, TB_PUSH_PULL_EL84_POS) with {
	fs = float(ma.SR);
	b0 = fs * (fs * (fs * (-6.57377333665727e-19 * fs - 4.01607201070155e-17) - 4.37865590304316e-21) + 1.22291757237028e-24);
	b1 = fs * (pow(fs, 2)*(2.62950933466291e-18 * fs + 8.03214402140309e-17) + 2.44583514474056e-24);
	b2 = pow(fs, 2)*(-3.94426400199436e-18 * pow(fs, 2) + 8.75731180608631e-21);
	b3 = fs * (pow(fs, 2)*(2.62950933466291e-18 * fs - 8.03214402140309e-17) - 2.44583514474056e-24);
	b4 = fs * (fs * (fs * (-6.57377333665727e-19 * fs + 4.01607201070155e-17) - 4.37865590304316e-21) - 1.22291757237028e-24);
	a0 = fs * (fs * (fs * (1.86957288162412e-19 * fs + 6.82010407286301e-17) + 4.53454013961264e-15) + 6.51104139918237e-14) + 4.73642640396319e-17;
	a1 = fs * (pow(fs, 2)*(-7.47829152649647e-19 * fs - 1.3640208145726e-16) + 1.30220827983647e-13) + 1.89457056158527e-16;
	a2 = pow(fs, 2)*(1.12174372897447e-18 * pow(fs, 2) - 9.06908027922528e-15) + 2.84185584237791e-16;
	a3 = fs * (pow(fs, 2)*(-7.47829152649647e-19 * fs + 1.3640208145726e-16) - 1.30220827983647e-13) + 1.89457056158527e-16;
	a4 = fs * (fs * (fs * (1.86957288162412e-19 * fs - 6.82010407286301e-17) + 4.53454013961264e-15) - 6.51104139918237e-14) + 4.73642640396319e-17;
};

feedbackCircuit = presenceFilter : *(feedback) with {
	presence = hslider("[0]Presence[style:knob]", 0, -15, 15, 0.1);
	presenceFilter = wa.peaking2(2000, presence, 1, 0) : wa.peaking2(4000, presence, 1, 0);
	feedback = hslider("[1]Feedback[label:Feedback][style:knob]", 0, -0.4, 1, 0.01) : si.smoo;
};

process = hgroup("PowerAmp", (+ : amp : fi.dcblocker) ~ feedbackCircuit : *(hslider("[2]Volume[style:knob]", 1.0, 0.0, 2.0, 0.1)));