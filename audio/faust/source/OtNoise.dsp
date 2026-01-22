//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "Noise";
declare name "Noise";
declare category "Noise";
declare license "MIT";

import("stdfaust.lib");

select = checkbox("[0][off: White][on: Pink]Type");
volume = hslider("[1]Volume", 0.0, 0.0, 1.0, 0.01);

process = no.noise, no.pink_noise : select2(select) * volume;
