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
	(os.triangle(freq) + os.sawtooth(freq)) / 1.5,
	os.sawtooth(freq),
	os.square(freq),
	os.pulsetrain(freq, 0.7) : ba.selectn(7, wf);

lfo(wf, freq) =
	0,
	os.oscsin(freq),
	os.lf_triangle(freq),
	(os.lf_triangle(freq) + os.lf_saw(freq)) / 1.5,
	os.lf_saw(freq),
	os.lf_squarewave(freq),
	os.lf_pulsetrain(freq, 0.7) : ba.selectn(7, wf);

vcoGroup(x) = hgroup("[0]Vco", x);
vco1WaveForm = vcoGroup(hslider("[1]WaveForm1[style:knob]", 1, 0, 7, 1));
vco2WaveForm = vcoGroup(hslider("[2]WaveForm2[style:knob]", 1, 0, 7, 1));
vco1Frequency = vcoGroup(hslider("[3]Frequency1[style:knob]", 440, 60, 10000, 1));
vco2Frequency = vcoGroup(hslider("[3]Frequency2[style:knob]", 440, 60, 10000, 1));

mixerGroup(x) = hgroup("[10]Mixer", x);
volume1 = mixerGroup(hslider("[11]Osc1[style:knob]", 0, 0, 1, 0.01));
volume2 = mixerGroup(hslider("[12]Osc2[style:knob]", 0, 0, 1, 0.01));
white = mixerGroup(hslider("[13]White[style:knob]", 0, 0, 1, 0.01));
pink = mixerGroup(hslider("[14]Pink[style:knob]", 0, 0, 1, 0.01) * 4);
mixer(s1, s2, s3, s4) = ((s1 * volume1) + (s2 * volume2) + (s3 * white) + (s4 * pink)) / 4;

vcfGroup(x) = hgroup("[20]Vcf", x);
cutoff = vcfGroup(vslider("[21]Cutoff[style:knob][format: %.0fhz]", 800, 80, 8000, 0.1)) : si.smoo;
resonance = vcfGroup(vslider("[23]Res[style:knob]", 0.5, 0, 1, 0.01));
envMod = vcfGroup(vslider("[24]Env[style:knob]", 0, 0, 2, 0.01));
lfoMod = vcfGroup(vslider("[25]Lfo[style:knob]", 0, 0, 2, 0.01));
attackVcf = vcfGroup(vslider("[26]Attack[format: %.3fs][style:knob][scale:log]", 0.01, 0, 10, 0.1));
decayVcf = vcfGroup(vslider("[27]Decay[format: %.3fs][style:knob][scale:log]", 0.05, 0, 10, 0.1));
sustainVcf = vcfGroup(vslider("[28]Sustain[format: %.2f][style:knob]", 0.8, 0, 1, 1));
releaseVcf = vcfGroup(vslider("[29]Release[format: %.3fs][style:knob][scale:log]", 0.03, 0, 10, 0.1));
gateVcf = vcfGroup(button("[30]Gate"));
lfoWaveForm = vcfGroup(hslider("[31]WaveForm[style:knob]", 1, 0, 7, 1));
lfoFrequency = vcfGroup(hslider("[32]Freq[style:knob][format: %.1fhz]", 2, 0.1, 100, 0.1));
envelopeVcf = en.adsre(attackVcf, decayVcf, sustainVcf, releaseVcf, gateVcf);
adjustedCutoff = cutoff * (2 ^ (envMod * envelopeVcf + lfoMod * lfo(lfoWaveForm, lfoFrequency)));
vcf = ve.lowpassLadder4(resonance * 4, adjustedCutoff);

vcaGroup(x) = hgroup("[30]Vca", x);
attackVca = vcaGroup(vslider("[31]Attack[format: %.3fs][style:knob][scale:log]", 0.01, 0, 10, 0.1));
decayVca = vcaGroup(vslider("[32]Decay[format: %.3fs][style:knob][scale:log]", 0.05, 0, 10, 0.1));
sustainVca = vcaGroup(vslider("[33]Sustain[format: %.2f][style:knob]", 0.8, 0, 1, 1));
releaseVca = vcaGroup(vslider("[34]Release[format: %.3fs][style:knob][scale:log]", 0.03, 0, 10, 0.1));
gateVca = vcaGroup(button("[35]Gate"));
vca = en.adsre(attackVca, decayVca, sustainVca, releaseVca, gateVca);

process =
	vco(vco1WaveForm, vco1Frequency),
	vco(vco2WaveForm, vco2Frequency),
	no.noise,
	no.pink_noise : mixer : vcf * vca;
