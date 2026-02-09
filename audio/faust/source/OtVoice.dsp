//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Voice";
declare category "Generator";
declare license "MIT";

import("stdfaust.lib");

osc(freq, wf) =
	0,
	os.oscsin(freq),
	os.triangle(freq),
	(os.triangle(freq) + os.sawtooth(freq)) / 1.5,
	os.sawtooth(freq),
	os.square(freq),
	os.pulsetrain(freq, 0.7) : ba.selectn(7, wf);

vcoGroup(x) = hgroup("[0]Vco", x);
wf1 = vcoGroup(hslider("[1]WaveForm1[style:knob]", 1, 0, 7, 1));
wf2 = vcoGroup(hslider("[2]WaveForm2[style:knob]", 1, 0, 7, 1));
freq1 = vcoGroup(hslider("[3]Freq1[style:knob]", 440, 60, 10000, 1));
freq2 = vcoGroup(hslider("[3]Freq2[style:knob]", 440, 60, 10000, 1));

mixerGroup(x) = hgroup("[10]Mixer", x);
volume1 = mixerGroup(hslider("[11]Osc1[style:knob]", 0, 0, 1, 0.01));
volume2 = mixerGroup(hslider("[12]Osc2[style:knob]", 0, 0, 1, 0.01));
white = mixerGroup(hslider("[13]White[style:knob]", 0, 0, 1, 0.01));
pink = mixerGroup(hslider("[14]Pink[style:knob]", 0, 0, 1, 0.01) * 4);
mixer(s1, s2, s3, s4) = ((s1 * volume1) + (s2 * volume2) +( s3 * white) + (s4 * pink)) / 4;

vcaGroup(x) = hgroup("[20]Vca", x);
attackVca = vcaGroup(vslider("[21]Attack[format: %.3fs][style:knob]", 0.01, 0, 10, 0.1));
decayVca = vcaGroup(vslider("[22]Decay[format: %.3fs][style:knob]", 0.05, 0, 10, 0.1));
sustainVca = vcaGroup(vslider("[23]Sustain[format: %.2f][style:knob]", 0.8, 0, 1, 1));
releaseVca = vcaGroup(vslider("[24]Release[format: %.3fs][style:knob]", 0.03, 0, 10, 0.1));
gate = vcaGroup(button("[25]Gate"));
vca = en.adsre(attackVca, decayVca, sustainVca, releaseVca, gate);

process = osc(freq1, wf1), osc(freq2, wf2), no.noise, no.pink_noise : mixer * vca;
