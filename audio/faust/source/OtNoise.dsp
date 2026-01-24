//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Noise";
declare category "Noise";
declare license "MIT";

import("stdfaust.lib");

select = hslider("[0]Type", 0.0, 0.0, 1.0, 1.0);
volume = hslider("[1]Volume[style:knob]", 0.0, 0.0, 1.0, 0.01);

process = no.noise, no.pink_noise : hgroup("", select2(select) * volume);
