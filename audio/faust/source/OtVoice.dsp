//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Voice";
declare category "Generator";
declare license "MIT";

import("stdfaust.lib");

vco(wf, freq) =
	0,
	os.oscsin(freq),
	os.triangle(freq),
	os.triangle(freq) * 0.75 + os.sawtooth(freq) * 0.25,
	os.sawtooth(freq),
	os.square(freq),
	os.pulsetrain(freq, 0.7) : ba.selectn(7, wf);

lfo(wf, freq) =
	0,
	os.oscsin(freq),
	os.lf_triangle(freq),
	os.lf_triangle(freq) * 0.75 + os.lf_saw(freq) * 0.25,
	os.lf_saw(freq),
	os.lf_squarewave(freq),
	os.lf_pulsetrain(freq, 0.7) : ba.selectn(7, wf);

vcoGroup(x) = hgroup("[0]Vco", x);
vco1WaveForm = vcoGroup(hslider("[1]WaveForm1[style:knob]", 1, 0, 7, 1));
vco2WaveForm = vcoGroup(hslider("[2]WaveForm2[style:knob]", 1, 0, 7, 1));
vco1Frequency = vcoGroup(hslider("[3]Frequency1[style:knob]", 440, 60, 10000, 1));
vco2Frequency = vcoGroup(hslider("[4]Frequency2[style:knob]", 440, 60, 10000, 1));
vco1Tuning = vcoGroup(hslider("[5]Tuning1[style:knob]", 0, -4800, 4800, 0.01));
vco2Tuning = vcoGroup(hslider("[6]Tuning2[style:knob]", 0, -4800, 4800, 0.01));

mixerGroup(x) = hgroup("[10]Mixer", x);
volume1 = mixerGroup(hslider("[11]Osc1[style:knob]", 0, 0, 1, 0.01));
volume2 = mixerGroup(hslider("[12]Osc2[style:knob]", 0, 0, 1, 0.01));
white = mixerGroup(hslider("[13]White[style:knob]", 0, 0, 1, 0.01));
pink = mixerGroup(hslider("[14]Pink[style:knob]", 0, 0, 1, 0.01) * 4);
mixer(s1, s2, s3, s4) = ((s1 * volume1) + (s2 * volume2) + (s3 * white) + (s4 * pink)) / 4;

vcfGroup(x) = hgroup("[20]Vcf", x);
cutoff = vcfGroup(vslider("[21]Cutoff[style:knob][format: %.0fhz]", 800, 80, 8000, 0.1)) : si.smoo;
resonance = vcfGroup(vslider("[22]Res[style:knob]", 0.5, 0, 1, 0.01));
attackVcf = vcfGroup(vslider("[23]Attack[format: %.3fs][style:knob][scale:log]", 0.01, 0, 10, 0.1));
decayVcf = vcfGroup(vslider("[24]Decay[format: %.3fs][style:knob][scale:log]", 0.05, 0, 10, 0.1));
sustainVcf = vcfGroup(vslider("[25]Sustain[format: %.2f][style:knob]", 0.8, 0, 1, 1));
releaseVcf = vcfGroup(vslider("[26]Release[format: %.3fs][style:knob][scale:log]", 0.03, 0, 10, 0.1));
gateVcf = vcfGroup(button("[27]Gate"));
envMod = vcfGroup(vslider("[28]EnvMod[style:knob]", 0, 0, 2, 0.01));
envPower = vcfGroup(hslider("[29]EnvPower", 0, 0, 1, 1));
lfoWaveForm = vcfGroup(hslider("[30]WaveForm[style:knob]", 1, 0, 7, 1));
lfoFrequency = vcfGroup(hslider("[31]LfoFreq[style:knob][format: %.1fhz]", 2, 0.1, 100, 0.1));
lfoMod = vcfGroup(vslider("[32]LfoMod[style:knob]", 0, 0, 2, 0.01));
lfoPower = vcfGroup(hslider("[33]LfoPower", 0, 0, 1, 1));
envelopeVcf = envPower * en.adsre(attackVcf, decayVcf, sustainVcf, releaseVcf, gateVcf);
adjustedCutoff = cutoff * (2 ^ (envMod * envelopeVcf + lfoPower * lfoMod * lfo(lfoWaveForm, lfoFrequency)));
vcf = ve.lowpassLadder4(resonance * 4, adjustedCutoff);

vcaGroup(x) = hgroup("[40]Vca", x);
attackVca = vcaGroup(vslider("[41]Attack[format: %.3fs][style:knob][scale:log]", 0.01, 0, 10, 0.1));
decayVca = vcaGroup(vslider("[42]Decay[format: %.3fs][style:knob][scale:log]", 0.05, 0, 10, 0.1));
sustainVca = vcaGroup(vslider("[43]Sustain[format: %.2f][style:knob]", 0.8, 0, 1, 1));
releaseVca = vcaGroup(vslider("[44]Release[format: %.3fs][style:knob][scale:log]", 0.03, 0, 10, 0.1));
gateVca = vcaGroup(button("[45]Gate"));
powerVca = vcaGroup(hslider("[46]Power", 1, 0, 1, 1));
vca = powerVca * en.adsre(attackVca, decayVca, sustainVca, releaseVca, gateVca);

process =
	vco(vco1WaveForm, vco1Frequency * ba.cent2ratio(vco1Tuning)),
	vco(vco2WaveForm, vco2Frequency * ba.cent2ratio(vco2Tuning)),
	no.noise,
	no.pink_noise : mixer : vcf * vca;
