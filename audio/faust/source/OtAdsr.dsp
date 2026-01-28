//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "ADSR";
declare category "Envelope";
declare license "MIT";

import("stdfaust.lib");

attack = vslider("[1]Attack[format: %.2fms][style:knob]", 1, 0, 10, 0.1);
decay = vslider("[2]Decay[format: %.2fms][style:knob]", 1, 0, 10, 0.1);
sustain = vslider("[3]Sustain[format: %.2f%%][style:knob]", 0.8, 0, 1, 1);
release = vslider("[4]Release[format: %.2fms][style:knob]", 1, 0, 10, 0.1);

process(gate) = hgroup("ADSR", en.adsre(attack * 0.001, decay * 0.001, sustain, release * 0.001, gate));
