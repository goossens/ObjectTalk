//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Noise";
declare category "Noise";
declare license "MIT";

import("stdfaust.lib");

white = hslider("[0]White[style:knob]", 0.0, 0.0, 1.0, 0.01);
pink = hslider("[1]Pink[style:knob]", 0.0, 0.0, 1.0, 0.01);

process = hgroup("Noise", no.noise * white + no.pink_noise * pink);
