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

oscGroup(x) = hgroup("", x);
wf = hslider("[1]WaveForm[style:radio{'Name0':value0;'Name1':value1}]", 0, 0, 4, 0.1);
freq = oscGroup(hslider("[2]Freq[style:knob]", 440, 60, 6000, 1));
pw = oscGroup(hslider("[3]PW[style:knob]", 0.5, 0, 1, 0.01));
gain = oscGroup(hslider("[4]Gain[style:knob]", 7, 0, 11, 0.01));

process = vgroup("", osc(freq, pw, wf) * (gain / 11));