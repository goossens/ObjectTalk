//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Chorus";
declare category "Effect";
declare license "MIT";

import("music.lib");

level = vslider("[0]Level[style:knob]", 0.5, 0, 1, 0.01);
freq = vslider("[1]Freq[style:knob]", 2, 0, 10, 0.01);
dtime = vslider("[2]Delay[style:knob]", 0.025, 0, 0.2, 0.001);
depth = vslider("[3]Depth[style:knob]", 0.02, 0, 1, 0.001);

tblosc(n, f, freq, mod) = (1 - d) * rdtable(n, wave, i & (n - 1)) + d * rdtable(n, wave, (i + 1) & (n - 1)) with {
	wave = time * (2.0 *PI) / n : f;
	phase = freq / SR : (+ : decimal) ~ _;
	modphase = decimal(phase + mod/(2 * PI)) * n;
	i = int(floor(modphase));
	d = decimal(modphase);
};

chorus(d,freq,depth) = fdelay(1 << 16, t) with {
	t = SR * d / 2 * (1 + depth * tblosc(1 << 16, sin, freq, 0));
};

process = hgroup("Chorus", (c, c)) with {
	c(x) = x + level * chorus(dtime, freq, depth, x);
};
