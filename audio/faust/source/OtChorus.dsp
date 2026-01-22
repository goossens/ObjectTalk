//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

declare id "chorus";
declare name "Chorus";
declare category "Effect";
declare license "MIT";

import("music.lib");

level	= hslider("[0]Level", 0.5, 0, 1, 0.01);
freq	= hslider("[1]Freq", 2, 0, 10, 0.01);
dtime	= hslider("[2]Delay", 0.025, 0, 0.2, 0.001);
depth	= hslider("[3]Depth", 0.02, 0, 1, 0.001);

tblosc(n,f,freq,mod)	= (1-d)*rdtable(n,wave,i&(n-1)) +
			  d*rdtable(n,wave,(i+1)&(n-1))
with {
	wave	 	= time*(2.0*PI)/n : f;
	phase		= freq/SR : (+ : decimal) ~ _;
	modphase	= decimal(phase+mod/(2*PI))*n;
	i		= int(floor(modphase));
	d		= decimal(modphase);
};

chorus(d,freq,depth)	= fdelay(1<<16, t)
with {	t		= SR*d/2*(1+depth*tblosc(1<<16, sin, freq, 0)); };

process			= vgroup("chorus", (c, c))
with { c(x) = x+level*chorus(dtime,freq,depth,x); };