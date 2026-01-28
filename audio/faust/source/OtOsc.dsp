//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Oscillator";
declare category "Generator";
declare license "MIT";

import("stdfaust.lib");

osc(freq, pw, wf) =
	os.triangle(freq),
	(os.triangle(freq) + os.sawtooth(freq)) / 2,
	os.sawtooth(freq),
	os.pulsetrain(freq, pw) : ba.selectn(4, wf);

wf = hslider("[1]WaveForm", 0, 0, 4, 1);
pw = hslider("[2]PW", 0.5, 0, 1, 0.01);
modGain = hslider("[3]Mod", 7, 0, 10, 0.01);
gain = hslider("[4]Gain", 7, 0, 11, 0.01);

process(freq, mod) = osc(freq + mod * modGain / 10, pw, wf) * (gain / 11);
