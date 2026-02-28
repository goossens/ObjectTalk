//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Tone Stack";
declare category "Equalizer";
declare license "MIT";

process = component("tonestacks.lib").soldano(t, m, l) with {
	l = vslider("[0]Bass[style:knob]", 0.5, 0, 1, 0.01);
	m = vslider("[1]Middle[style:knob]", 0.5, 0, 1, 0.01);
	t = vslider("[2]Treble[style:knob]", 0.5, 0, 1, 0.01);
};
