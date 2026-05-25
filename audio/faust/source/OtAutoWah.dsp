//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "WahWah";
declare category "Modulation";
declare license "MIT";

import("stdfaust.lib");

process = hgroup("Auto Wah", ve.autowah(level) with {
	level = vslider("[0]Level[style:knob]", 0.4, 0, 1, 0.01) : si.smoo;
});
