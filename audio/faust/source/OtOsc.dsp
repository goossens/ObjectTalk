//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Oscillator";
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

wf = hslider("[1]WaveForm", 1, 0, 7, 1);

cv2hz(cv) = 440 * 2 ^ cv;
process(cv) = osc(cv2hz(cv), wf);
