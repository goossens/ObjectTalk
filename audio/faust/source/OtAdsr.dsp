//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "ADSR";
declare category "Envelope";
declare license "MIT";

import("stdfaust.lib");

attack = vslider("[1]Attack[format: %.3fs][style:knob][scale:log]", 0.01, 0, 10, 0.1);
decay = vslider("[2]Decay[format: %.3fs][style:knob][scale:log]", 0.05, 0, 10, 0.1);
sustain = vslider("[3]Sustain[format: %.2f][style:knob]", 0.8, 0, 1, 1);
release = vslider("[4]Release[format: %.3fs][style:knob][scale:log]", 0.03, 0, 10, 0.1);
adsr(gate) = en.adsre(attack, decay, sustain, release, gate);

process(gate) = hgroup("ADSR", adsr(gate));
