//	TextDiff - A syntax highlighting text diff widget for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	line break class types for codepoints
//

enum class LBC : char {
	ai,			// ambiguous (alphabetic or ideograph)
	ak,			// aksara
	al,			// alphabetic
	ap,			// aksara pre-base
	as,			// aksara start
	b2,			// break on either side (but not pair)
	ba,			// break after
	bb,			// break before
	bk,			// break (mandatory)
	cb,			// contingent break
	cj,			// conditional Japanese starter
	cl,			// closing punctuation
	cm,			// combining marks
	cp,			// closing parenthesis
	cr,			// carriage return
	eb,			// emoji base
	em,			// emoji modifier
	ex,			// exclamation/interrogation
	gl,			// glue
	h2,			// hangul lv
	h3,			// hangul lvt
	hh,			// unambiguous hyphen
	hl,			// hebrew letter
	hy,			// hyphen
	id,			// ideographic
	in,			// inseparable characters
	is,			// infix separator
	jl,			// hangul l jamo
	jt,			// hangul t jamo
	jv,			// hangul v jamo
	lf,			// line feed
	nl,			// next line
	ns,			// non-starters
	nu,			// numeric
	op,			// opening punctuation
	po,			// postfix
	pr,			// prefix
	qu,			// ambiguous quotation
	ri,			// regional indicator
	sa,			// complex context dependent (South East Asian)
	sg,			// surrogate (non-tailorable)
	sp,			// space
	sy,			// symbols allowing break after
	vf,			// virama final
	vi,			// virama
	wj,			// word joiner
	xx,			// unknown
	zw,			// zero-width space
	zwj,		// zero width joiner

	sot,		// pseudo class - start of text
	eot,		// pseudo class - end of text
	undefined	// pseudo value for undefined class
};
