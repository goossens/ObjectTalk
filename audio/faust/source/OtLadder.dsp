//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare name "Ladder";
declare category "Filter";
declare license "MIT";

import("stdfaust.lib");

ladderGroup(x) = hgroup("[0]Ladder", x);

filterGroup(x) = ladderGroup(hgroup("[1]Filter", x));
cutoff = filterGroup(vslider("[2]Cutoff[style:knob]", 0, -4, 4, 0.1));
emphasis = filterGroup(vslider("[3]Emphasis[style:knob]", 1, 0.707, 25.0, 0.001));
contour = filterGroup(vslider("[4]Contour[style:knob]", 0.0, 0.0, 1.0, 0.01));

controlGroup(x) = ladderGroup(vgroup("[5]Control", x));
oneControl = controlGroup(checkbox("[6]One[labels:1|]"));
twoControl = controlGroup(checkbox("[7]Two[labels:2|]") * 2.0);

process(signal, cv, envelope, modulation) = signal : ve.moogLadder(freq, emphasis) with {
	kbControl = (oneControl + twoControl) / 3.0 * cv;
	cv2hz(x) = 440 * 2 ^ x;
	freq = cv2hz(cutoff + (envelope * contour) + (modulation * contour) + kbControl);
};
