//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/micbuffa/FaustPowerAmp

import("stdfaust.lib");

// distortion parameters
pdrive = hslider("Drive[style:knob]", 4.0, -10.0, 10.0, 0.001) : si.smooth(0.995);
psat = hslider("Saturation[label:Sat][style:knob]", 1.0, 0.0, 1.0, 0.001) : si.smooth(0.995);
pcurve = hslider("Curve[style:knob]", 1.0, 0.1, 4.0, 0.001) : si.smooth(0.995);

// approximation of the `tanh` function for computational improvement
// see: https://www.musicdsp.org/en/latest/Other/238-rational-tanh-approximation.html
tanh(x) = x * (27 + x * x) / (27 + 9 * x * x);
transfer(x) = tanh(pcurve * x) / tanh(pcurve);

drive(x) = x : *(pdrive) : +(fol(x)) : max(-3) : min(3) with {
	fol = an.amp_follower(0.04);
};

waveshaper(x) = x <: _, tap(x) : *(1.0 - psat), *(psat) : + : fi.tf1(b0(x), b1(x), a1(x)) with {
	b0(x) = m(x);
	b1(x) = 1.0;
	a1(x) = m(x);
	m(x) = drive(x) : transfer : *(0.24);
	tap(x) = m(x);
};

tf2s(b2, b1, b0, a1, a0, w1) = fi.tf2(b0d, b1d, b2d, a1d, a2d) with {
	c = 1 / tan(w1 * 0.5 / ma.SR) : si.smooth(0.995);
	csq = c * c;
	d = a0 + a1 * c + csq;
	b0d = (b0 + b1 * c + b2 * csq) / d;
	b1d = 2 * (b0 - b2 * csq) / d;
	b2d = (b0 - b1 * c + b2 * csq) / d;
	a1d = 2 * (a0 - csq) / d;
	a2d = (a0 - a1 * c + csq) / d;
};

resonlp(fc, Q, gain) = tf2s(b2,b1,b0,a1,a0,wc) with {
	wc = 2 * ma.PI * fc;
	a1 = 1/Q;
	a0 = 1;
	b2 = 0;
	b1 = 0;
	b0 = gain;
};

feedbackCircuit = presence:*(gainNFL)  with {
	p1gain = hslider("[3]Presence[label:Pres][style:knob]", 0, -15, 15, 0.1);
	presence =wa.peaking2(2000, p1gain, 1, 0) : wa.peaking2(4000, p1gain, 1, 0);
	gainNFL = hslider("[4]Feedback[label:Fback][style:knob]", -0.4, -0.8, 1, 0.01) :  si.smoo;
};

process = hgroup("PowerAmp", (+ : waveshaper : fi.dcblocker) ~ feedbackCircuit : *(4.0));
