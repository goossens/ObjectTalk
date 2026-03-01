//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

import("stdfaust.lib");
import("OtTube.lib");

stage1(gain) =
	tubestage(TB_12AY7_68k, 250.0, 40.0, 86.0, 2700.0, 1.581656) :
	*(gain) :
	fi.lowpass(1, 6531.0) :
	tubestage(TB_12AY7_250k, 250.0, 40.0, 132.0, 1500.0, 1.204285) :
	*(gain);

stage2(volume) =
	fi.lowpass(1, 6531.0) :
	tubestage(TB_12AY7_250k, 250.0, 40.0, 194.0, 820.0, 0.840703) :
	*(volume);

process = hgroup("preamp", stage1(gain) : stage2(volume) with {
	gain = vslider("[0]Gain[style:knob][format: %.1fdB]", -6, -20, 20, 0.1) : ba.db2linear : si.smooth(0.999);
	volume = vslider("[1]Volume[style:knob][format: %.1fdB]", 0, -20.0, 20.0, 0.1) : ba.db2linear : si.smooth(0.999);
});
