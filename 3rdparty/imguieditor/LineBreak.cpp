//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/cto-af/linebreak


//
//	Include files
//

#include "LineBreakTables.h"
#include "TextEditor.h"


//
//	lineBreakRangeFind
//

template <typename T, typename C>
LBC lineBreakRangeFind(const T& table, C codepoint) {
	auto low = std::begin(table);
	auto high = std::end(table);

	while (low <= high) {
		auto mid = low + (high - low) / 2;

		if (codepoint >= mid->low && codepoint <= mid->high) {
			return mid->lbc;

		} else if (codepoint < mid->low) {
			high = mid - 1;

		} else {
			low = mid + 1;
		}
	}

	return LBC::undefined;
}


//
//	getLineBreakClass
//

static LBC getLineBreakClass(ImWchar codepoint) {
	LBC lbc;

	// handle simple 8-bit ASCII codepoints
	if (codepoint < 256) {
		return lineBreak8[codepoint];
	}

#if defined(IMGUI_USE_WCHAR32)
	if (codepoint >= 0x10000) {
		lbc = lineBreakRangeFind(lineBreak32, static_cast<char32_t>(codepoint));

	} else
#endif

	{
		lbc = lineBreakRangeFind(lineBreak16, static_cast<char16_t>(codepoint));
	}

	if (lbc == LBC::undefined) {
		if ((codepoint >= 0x3400 && codepoint <= 0x4DBF) ||
			(codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||
			(codepoint >= 0xF900 && codepoint <= 0xFAFF)

#if defined(IMGUI_USE_WCHAR32)
			||
			(codepoint >= 0x20000 && codepoint <= 0x2FFFD) ||
			(codepoint >= 0x30000 && codepoint <= 0x3FFFD) ||

			(codepoint >= 0x1F000 && codepoint <= 0x1FAFF) ||
			(codepoint >= 0x1FC00 && codepoint <= 0x1FFFD)
#endif
		) {

			lbc = LBC::id;

		} else if (codepoint >= 0x20A0 && codepoint <= 0x20CF) {
			lbc = LBC::pr;

		} else {
			lbc = LBC::xx;
		}
	}

	// LB1: assign a line breaking class to each code point of the input
	// resolve AI, CB, CJ, SA, SG, and XX into other line breaking classes
	if (lbc == LBC::ai || lbc == LBC::sg || lbc == LBC::xx) {
		lbc = LBC::al;

	} else if (lbc == LBC::sa) {
		lbc = LBC::al;

	} else if (lbc == LBC::cj) {
		lbc = LBC::ns;
	}

	return lbc;
}


//
//	State machine status
//

static constexpr size_t invalidPos = std::numeric_limits<std::size_t>::max();
static constexpr ImWchar dotCircle = 0x25CC;

struct LineBreakGlyph {
	LineBreakGlyph() = default;
	LineBreakGlyph(ImWchar codepoint, LBC cls, size_t pos) : codepoint(codepoint), cls(cls), pos(pos) {}
	ImWchar codepoint = 0;
	LBC cls = LBC::sot;
	size_t pos = invalidPos;
	bool ignored = false;
};

struct LineBreakState {
	TextEditor::Glyph* glyphs;
	size_t size;
	LineBreakGlyph previous;
	LineBreakGlyph current;
	LineBreakGlyph next;
	bool spaces = false;
	bool lb8 = false;
	size_t ri = 0;

	// move to the next state
	inline void push(LineBreakGlyph step) {
		if (next.ignored) {
			current.pos = next.pos;

		} else {
			previous = current;
			current = next;
		}

		next = step;
	}

	// get the codepoint at specified location
	ImWchar getCodepoint(size_t pos) {
		if (pos < size) {
			return glyphs[pos].codepoint;

		} else {
			return 0;
		}
	}

	// get the line break class at specified location
	LBC getClass(size_t pos) {
		if (pos < size) {
			return getLineBreakClass(glyphs[pos].codepoint);

		} else {
			return LBC::eot;
		}
	}
};


//
//	Support functions
//

static inline bool isPf(ImWchar codepoint) {
	if (codepoint < 0x00bb) {
		return false;

	} else {
		static std::unordered_set<ImWchar> Pf = {
			0x00bb, 0x2019, 0x201d, 0x203a, 0x2e03, 0x2e05, 0x2e0a, 0x2e0d, 0x2e1d, 0x2e21
		};

		return Pf.find(codepoint) != Pf.end();
	}
}


static inline bool isPi(ImWchar codepoint) {
	if (codepoint < 0x00ab) {
		return false;

	} else {
		static std::unordered_set<ImWchar> Pi = {
			0x00ab, 0x2018, 0x201b, 0x201c, 0x201f, 0x2039, 0x2e02, 0x2e04, 0x2e09, 0x2e0c, 0x2e1c, 0x2e20
		};

		return Pi.find(codepoint) != Pi.end();
	}
}


static inline bool isAkCircleAs(LineBreakGlyph& lbg) {
	return (lbg.cls == LBC::ak) || (lbg.codepoint == 0x25CC) || (lbg.cls == LBC::as);
}


//
//	Line break rules
//
//	Partly ported from on https://github.com/cto-af/linebreak
//

static inline TextEditor::BreakOption lb2(LineBreakState& state) {
	// LB2: never break at the start of text
	// sot ×
	if (state.current.cls == LBC::sot && state.next.cls != LBC::eot) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb3(LineBreakState& state) {
	// LB3: always break at the end of text
	// ! eot
	if (state.next.cls == LBC::eot) {
		return TextEditor::BreakOption::mustBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb4(LineBreakState& state) {
	// LB4: always break after hard line breaks
	// BK !
	if (state.current.cls == LBC::bk) {
		return TextEditor::BreakOption::mustBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb5(LineBreakState& state) {
	// LB5: treat CR followed by LF, as well as CR, LF, and NL as hard line breaks
	// CR × LF
	// CR !
	// LF !
	// NL !
	switch (state.current.cls) {
		case LBC::cr:
			if (state.next.cls == LBC::lf) {
				return TextEditor::BreakOption::noBreak;
			}

			return TextEditor::BreakOption::mustBreak;

		case LBC::lf:
		case LBC::nl:
			return TextEditor::BreakOption::mustBreak;

		default:
			return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb6(LineBreakState& state) {
	// LB6: do not break before hard line breaks
	// × ( BK | CR | LF | NL )
	switch (state.next.cls) {
		case LBC::bk:
		case LBC::cr:
		case LBC::lf:
		case LBC::nl:
			return TextEditor::BreakOption::noBreak;

		default:
			return TextEditor::BreakOption::undefined;
	}
}

static inline TextEditor::BreakOption lbSpaceStop(LineBreakState& state) {
	// for rules that have "do not break within even with intervening spaces", such as LB15
	if (state.current.cls != LBC::ri) {
		state.ri = 0;
	}

	if (state.spaces) {
		if (state.next.cls != LBC::sp) {
			state.spaces = false;
		}

		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb7(LineBreakState& state) {
	// LB7: do not break before spaces or zero width space
	// × ZW
	if (state.next.cls == LBC::zw) {
		return TextEditor::BreakOption::noBreak;
	}

	// × SP
	if (state.next.cls == LBC::sp) {
		switch (state.current.cls) {
			case LBC::zw: // see LB8
			case LBC::op: // see LB14
			case LBC::qu: // see LB15
			case LBC::cl: // see LB16
			case LBC::cp: // see LB16
			case LBC::b2: // see LB17
				break;

			default:
				return TextEditor::BreakOption::noBreak;
		}
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb8(LineBreakState& state) {
	// LB8: break before any character following a zero-width space, even if one or more spaces intervene
	// ZW SP* ÷
	if (state.lb8) {
		state.lb8 = false;
		return TextEditor::BreakOption::allowBreak;

	} else if (state.current.cls == LBC::zw) {
		if (state.next.cls == LBC::sp) {
			state.lb8 = true;
			return TextEditor::BreakOption::noBreak;
		}

		return TextEditor::BreakOption::allowBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb8a(LineBreakState& state) {
	// LB8a: do not break after a zero width joiner
	// ZWJ ×
	if (state.current.cls == LBC::zwj) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb9(LineBreakState& state) {
	// LB9: do not break a combining character sequence; treat it as if it has the
	// line breaking class of the base character in all of the following rules
	static const std::unordered_set<LBC> BKCRLFNLSPZW = {LBC::bk, LBC::cr, LBC::lf, LBC::nl, LBC::sp, LBC::zw};

	// treat X (CM | ZWJ)* as if it were X
  	// where X is any line break class except BK, CR, LF, NL, SP, or ZW
	if (BKCRLFNLSPZW.find(state.current.cls) == BKCRLFNLSPZW.end() &&
		(state.next.cls == LBC::cm || state.next.cls == LBC::zwj)) {

		state.next.ignored = true;
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb10(LineBreakState& state) {
	// LB10: treat any remaining combining mark or ZWJ as AL
	if (state.current.cls == LBC::cm) {
		state.current.cls = LBC::al;
	}

	if (state.next.cls == LBC::cm) {
		state.next.cls = LBC::al;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb11(LineBreakState& state) {
	// LB11: do not break before or after word joiner and related characters
	// × WJ
	// WJ ×
	if (state.next.cls == LBC::wj || state.current.cls == LBC::wj) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb12(LineBreakState& state) {
	// LB12: do not break after NBSP and related characters
	// GL ×
	if (state.current.cls == LBC::gl) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb12a(LineBreakState& state) {
	// LB12a: do not break before NBSP and related characters, except after spaces and hyphens
	// [^SP BA HY HH] × GL
	if (state.next.cls == LBC::gl) {
		switch (state.current.cls) {
			case LBC::sp:
			case LBC::ba:
			case LBC::hy:
			case LBC::hh:
				return TextEditor::BreakOption::undefined;

			default:
				return TextEditor::BreakOption::noBreak;
		}
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb13(LineBreakState& state) {
	// LB13: do not break before ‘]’ or ‘!’ or ‘;’ or ‘/’, even after spaces
	// × CL
	// × CP
	// × EX
	// × SY
	switch (state.next.cls) {
		case LBC::cl:
		case LBC::cp:
		case LBC::ex:
		case LBC::sy:
			return TextEditor::BreakOption::noBreak;

		default:
			return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb14(LineBreakState& state) {
	// LB14: do not break after ‘[’, even after spaces
	// OP SP* ×
	if (state.current.cls == LBC::op) {
		if (state.next.cls == LBC::sp) {
			state.spaces = true;
		}

		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb15a(LineBreakState& state) {
	// LB15a: do not break after an unresolved initial punctuation that lies at the start of the line,
	// after a space, after opening punctuation, or after an unresolved quotation mark, even after spaces
	static const std::unordered_set<LBC> sotBKCRLFNLOPQUGLSPZW = {
		LBC::sot, LBC::bk, LBC::cr, LBC::lf, LBC::nl, LBC::op, LBC::qu, LBC::gl, LBC::sp, LBC::zw
	};

	// (sot | BK | CR | LF | NL | OP | QU | GL | SP | ZW) [\p{Pi}&QU] SP* ×
	if (sotBKCRLFNLOPQUGLSPZW.find(state.previous.cls) != sotBKCRLFNLOPQUGLSPZW.end() &&
		isPi(state.current.codepoint) &&
		state.current.cls == LBC::qu) {

		state.spaces = true;
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb15b(LineBreakState& state) {
	// LB15b: do not break before an unresolved final punctuation that lies at the end of the line,
	// before a space, before a prohibited break, or before an unresolved quotation mark, even after spaces
	static const std::unordered_set<LBC> SPGLWJCLQUCPEXISSYBKCRLFNLZW = {
		LBC::sp, LBC::gl, LBC::wj, LBC::cl, LBC::qu, LBC::cp, LBC::ex,
		LBC::is, LBC::sy, LBC::bk, LBC::cr, LBC::lf, LBC::nl, LBC::zw
	};

	// × [\p{Pf}&QU] ( SP | GL | WJ | CL | QU | CP | EX | IS | SY | BK | CR | LF | NL | ZW | eot)
	if (isPf(state.next.codepoint) && state.next.cls == LBC::qu) {
		auto after = state.getClass(state.next.pos + 1);

		if (after == LBC::eot) {
			return TextEditor::BreakOption::noBreak;
		}

		if (SPGLWJCLQUCPEXISSYBKCRLFNLZW.find(after) == SPGLWJCLQUCPEXISSYBKCRLFNLZW.end()) {
			return TextEditor::BreakOption::noBreak;
		}
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb15c(LineBreakState& state) {
	// LB15c: Break before a decimal mark that follows a space, for instance, in ‘subtract .5’
	// SP ÷ IS NU
	if ((state.current.cls == LBC::sp) && (state.next.cls == LBC::is)) {
		if (state.getClass(state.next.pos + 1) == LBC::nu) {
			return TextEditor::BreakOption::allowBreak;
		}
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb15d(LineBreakState& state) {
	// LB15d: otherwise, do not break before ‘;’, ‘,’, or ‘.’, even after spaces
	// × IS
	if (state.next.cls == LBC::is) {
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb16(LineBreakState& state) {
	// LB16: do not break between closing punctuation and a nonstarter (lb=NS), even with intervening space
	// (CL | CP) SP* × NS
	if ((state.current.cls == LBC::cl) || (state.current.cls == LBC::cp)) {
		auto pos = state.next.pos;
		LBC cls;

		do {
			cls = state.getClass(pos++);
		} while (cls == LBC::sp);

		if (state.getClass(pos) == LBC::ns) {
			if (state.next.cls == LBC::sp) {
				state.spaces = true;
			}

			return TextEditor::BreakOption::noBreak;
		}

		if (state.next.cls == LBC::sp) {
			return TextEditor::BreakOption::noBreak;
		}
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb17(LineBreakState& state) {
	// LB17: do not break within ‘——’, even with intervening spaces
	// B2 SP* × B2
	if (state.current.cls == LBC::b2) {
		auto pos = state.next.pos;
		LBC cls;

		do {
			cls = state.getClass(pos++);
		} while (cls == LBC::sp);

		if (state.getClass(pos) == LBC::b2) {
			if (state.next.cls != LBC::sp) {
				return TextEditor::BreakOption::noBreak;
			}

			state.spaces = true;
			return TextEditor::BreakOption::noBreak;

		} else if (state.next.cls == LBC::sp) {
			return TextEditor::BreakOption::noBreak;
		}
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb18(LineBreakState& state) {
	// LB18: Break after spaces
	// SP ÷
	if (state.current.cls == LBC::sp) {
		return TextEditor::BreakOption::allowBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}

static inline TextEditor::BreakOption lb19(LineBreakState& state) {
	// LB19: do not break before non-initial unresolved quotation marks, such as ‘ ” ’ or ‘ " ’,
	// nor after non-final unresolved quotation marks, such as ‘ “ ’ ‘ " ’
	// × [ QU - \p{Pi} ]
	if ((state.next.cls == LBC::qu) && !isPi(state.next.codepoint)) {
		// Gc=Pi is initial punctuation
		return TextEditor::BreakOption::noBreak;
	}

	// [ QU - \p{Pf} ] ×
	if ((state.current.cls == LBC::qu) && !isPf(state.current.codepoint)) {
		// Gc=Pf is final punctuation
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb19a(LineBreakState& state) {
	// LB19a: unless surrounded by East Asian characters, do not break either side
	// [^$EastAsian] × QU
	if (!TextEditor::CodePoint::isEastAsian(state.current.codepoint) && (state.next.cls == LBC::qu)) {
		return TextEditor::BreakOption::noBreak;
	}

	// × QU ( [^$EastAsian] | eot )
	if (state.next.cls == LBC::qu) {
		auto afterCodepoint = state.getCodepoint(state.next.pos + 1);
		auto afterCls = state.getClass(state.next.pos + 1);

		if (afterCls == LBC::eot || !TextEditor::CodePoint::isEastAsian(afterCodepoint)) {
			return TextEditor::BreakOption::noBreak;
		}
	}

	// QU × [^$EastAsian]
	if ((state.current.cls == LBC::qu) && !TextEditor::CodePoint::isEastAsian(state.next.codepoint)) {
		return TextEditor::BreakOption::noBreak;
	}

	// ( sot | [^$EastAsian] ) qu ×
	if ((state.previous.cls == LBC::sot || !TextEditor::CodePoint::isEastAsian(state.previous.codepoint)) && state.current.cls == LBC::qu) {
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb20(LineBreakState& state) {
	// LB20: break before and after unresolved CB
	// ÷ CB
	// CB ÷
	if ((state.current.cls == LBC::cb) || (state.next.cls == LBC::cb)) {
		return TextEditor::BreakOption::allowBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb20a(LineBreakState& state) {
	// LB20a: do not break after a word-initial hyphen
	static const std::unordered_set<LBC> sotBKCRLFNLSPZWCBGL = {
		LBC::sot, LBC::bk, LBC::cr, LBC::lf, LBC::nl, LBC::sp, LBC::zw, LBC::cb, LBC::gl
	};

	// ( sot | BK | CR | LF | NL | SP | ZW | CB | GL ) ( HY | HH ) × ( AL | HL )
	if (sotBKCRLFNLSPZWCBGL.find(state.previous.cls) != sotBKCRLFNLSPZWCBGL.end() &&
		((state.current.cls == LBC::hy) || (state.current.cls == LBC::hh)) &&
		((state.next.cls == LBC::al) || (state.next.cls == LBC::hl))) {

		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb21(LineBreakState& state) {
	// LB21: do not break before hyphen-minus, other hyphens, fixed-width spaces, small kana, and other non-starters, or after acute accents
	// BB ×
	if (state.current.cls == LBC::bb) {
		return TextEditor::BreakOption::noBreak;
	}

	// × ( BA | HH | HY | NS )
	switch (state.next.cls) {
		case LBC::ba:
		case LBC::hh:
		case LBC::hy:
		case LBC::ns:
			return TextEditor::BreakOption::noBreak;

		default:
			return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb21a(LineBreakState& state) {
	// LB21a: do not break after the hyphen in Hebrew + Hyphen + non-Hebrew
	// HL (HY | HH) × [^HL]
	if ((state.previous.cls == LBC::hl) &&
		(state.current.cls == LBC::hy || state.current.cls == LBC::hh) &&
		state.next.cls != LBC::hl) {

		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb21b(LineBreakState& state) {
	// don’t break between Solidus and Hebrew letters
	// SY × HL
	if ((state.current.cls == LBC::sy) && (state.next.cls == LBC::hl)) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb22(LineBreakState& state) {
	// do not break before ellipses
	// × IN
	if (state.next.cls == LBC::in) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb23(LineBreakState& state) {
	// do not break between digits and letters
	switch (state.current.cls) {
		case LBC::al:
		case LBC::hl:
			// (AL | HL) × NU
			if (state.next.cls == LBC::nu) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		case LBC::nu:
			// NU × (AL | HL)
			if ((state.next.cls == LBC::al) || (state.next.cls == LBC::hl)) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		default:
			return TextEditor::BreakOption::undefined;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb23a(LineBreakState& state) {
	// LB23a: do not break between numeric prefixes and ideographs, or between ideographs and numeric postfixes
	static const std::unordered_set<LBC> IDEBEM = {LBC::id, LBC::eb, LBC::em};

	// PR × (ID | EB | EM)
	if ((state.current.cls == LBC::pr) && IDEBEM.find(state.next.cls) != IDEBEM.end()) {
		return TextEditor::BreakOption::noBreak;
	}

	// (ID | EB | EM) × PO
	if ((state.next.cls == LBC::po) && IDEBEM.find(state.current.cls) != IDEBEM.end()) {
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb24(LineBreakState& state) {
	// LB24: do not break between numeric prefix/postfix and letters, or between letters and prefix/postfix
	// (PR | PO) × (AL | HL)
	if ((state.current.cls == LBC::pr || state.current.cls == LBC::po) &&
		(state.next.cls == LBC::al || state.next.cls == LBC::hl)) {
		return TextEditor::BreakOption::noBreak;
	}

	// (AL | HL) × (PR | PO)
	if ((state.current.cls == LBC::al || state.current.cls == LBC::hl) &&
		(state.next.cls == LBC::pr || state.next.cls == LBC::po)) {
		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb25(LineBreakState& state) {
	// LB25: do not break numbers
	// approach: find the end of a matching run, then no-break everything as we go past it
	static const std::unordered_set<LBC> POPR = {LBC::po, LBC::pr};
	static const std::unordered_set<LBC> CLCP = {LBC::cl, LBC::cp};

	// NU ( SY | IS )* CL × PO
	// NU ( SY | IS )* CP × PO
	// NU ( SY | IS )* CL × PR
	// NU ( SY | IS )* CP × PR
	// NU ( SY | IS )* × PO
	// NU ( SY | IS )* × PR
	// NU ( SY | IS )* × NU
	size_t pos = invalidPos;

	if (POPR.find(state.next.cls) != POPR.end()) {
		if (CLCP.find(state.current.cls) != CLCP.end()) {
			pos = state.previous.pos;

		} else {
			pos = state.current.pos;
		}

	} else if (state.next.cls == LBC::nu) {
		pos = state.current.pos;
	}

	// as specified, possible ReDoS because of the backtracking
	if (pos != invalidPos) {
		while (pos != invalidPos) {
			switch(state.getClass(pos)) {
				case LBC::sy:
				case LBC::is:
					break;

				case LBC::nu:
					return TextEditor::BreakOption::noBreak;

				default:
					pos = 0;
			}

			pos = pos == 0 ? invalidPos : pos - 1;
		}
	}

	// PO × OP NU
	// PO × OP IS NU
	// PO × NU
	// PR × OP NU
	// PR × OP IS NU
	// PR × NU
	if ((state.current.cls == LBC::po) || ((state.current.cls == LBC::pr))) {
		if (state.next.cls == LBC::op) {
			auto after = state.getClass(state.next.pos + 1);

			if (after != LBC::eot) {
				if (after == LBC::nu) {
					return TextEditor::BreakOption::noBreak;

				} else if (after == LBC::is) {
					auto afterAfter = state.getClass(state.next.pos + 2);

					if (afterAfter == LBC::nu) {
						return TextEditor::BreakOption::noBreak;
					}
				}
			}

		} else if (state.next.cls == LBC::nu) {
			return TextEditor::BreakOption::noBreak;
		}
	}

	// HY × NU
	if ((state.current.cls == LBC::hy) && (state.next.cls == LBC::nu)) {
		return TextEditor::BreakOption::noBreak;
	}

	// IS × NU
	if ((state.current.cls == LBC::is) && (state.next.cls == LBC::nu)) {
		return TextEditor::BreakOption::noBreak;
	}

		return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb26(LineBreakState& state) {
	// LB26 do not break a Korean syllable
	static const std::unordered_set<LBC> JLJVH2H3 = {LBC::jl, LBC::jv, LBC::h2, LBC::h3};
	static const std::unordered_set<LBC> JVJT = {LBC::jv, LBC::jt};

	switch (state.current.cls) {
		case LBC::jl:
			// JL × (JL | JV | H2 | H3)
			if (JLJVH2H3.find(state.next.cls) != JLJVH2H3.end()) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		case LBC::jv:
		case LBC::h2:
			// (JV | H2) × (JV | JT)
			if (JVJT.find(state.next.cls) != JVJT.end()) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		case LBC::jt:
		case LBC::h3:
			// (JT | H3) × JT
			if (state.next.cls == LBC::jt) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		default:
		  return TextEditor::BreakOption::undefined;
  	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb27(LineBreakState& state) {
	// LB27: treat a Korean Syllable Block the same as LBC::id
	static const std::unordered_set<LBC> JLJVJTH2H3 = {LBC::jl, LBC::jv, LBC::jt, LBC::h2, LBC::h3};

	switch (state.current.cls) {
		case LBC::jl:
		case LBC::jv:
		case LBC::jt:
		case LBC::h2:
		case LBC::h3:
			// (JL | JV | JT | H2 | H3) × PO
			if (state.next.cls == LBC::po) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		case LBC::pr:
			// PR × (JL | JV | JT | H2 | H3)
			if (JLJVJTH2H3.find(state.next.cls) != JLJVJTH2H3.end()) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		default:
		  return TextEditor::BreakOption::undefined;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb28(LineBreakState& state) {
	// LB28 do not break between alphabetics (“at”)
	// (AL | HL) × (AL | HL)
	if ((state.current.cls == LBC::al || state.current.cls == LBC::hl) &&
		(state.next.cls == LBC::al || state.next.cls == LBC::hl)) {

		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb28a(LineBreakState& state) {
	// LB28a: do not break inside the orthographic syllables of Brahmic scripts
	// AP × (AK | [◌] | AS)
	if ((state.current.cls == LBC::ap) && isAkCircleAs(state.next)) {
		return TextEditor::BreakOption::noBreak;
	}

	// (AK | [◌] | AS) × (VF | VI)
	if (isAkCircleAs(state.current) && ((state.next.cls == LBC::vf) || (state.next.cls == LBC::vi))) {
		return TextEditor::BreakOption::noBreak;
	}

	// (AK | [◌] | AS) VI × (AK | [◌])
	if (isAkCircleAs(state.previous) &&
		state.current.cls == LBC::vi && (state.next.cls == LBC::ak || state.next.codepoint == dotCircle)) {

		return TextEditor::BreakOption::noBreak;
	}

	// (AK | [◌] | AS) × (AK | [◌] | AS) VF
	if (isAkCircleAs(state.current) &&
		isAkCircleAs(state.next) && (state.getClass(state.next.pos + 1) == LBC::vf)) {

		return TextEditor::BreakOption::noBreak;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb29(LineBreakState& state) {
	// LB29: do not break between numeric punctuation and alphabetics (“e.g.”)
	// IS × (AL | HL)
	if (state.current.cls == LBC::is && (state.next.cls == LBC::al || state.next.cls == LBC::hl)) {
		return TextEditor::BreakOption::noBreak;

	} else {
		return TextEditor::BreakOption::undefined;
	}
}


static inline TextEditor::BreakOption lb30(LineBreakState& state) {
	// LB30: do not break between letters, numbers, or ordinary symbols and opening or closing parentheses
	static const std::unordered_set<LBC> ALHLNU = {LBC::al, LBC::hl, LBC::nu};

	switch (state.current.cls) {
		case LBC::al:
		case LBC::hl:
		case LBC::nu:
			// (AL | HL | NU) × [OP-$EastAsian]
			if (state.next.cls == LBC::op && !TextEditor::CodePoint::isEastAsian(state.next.codepoint)) {
				return TextEditor::BreakOption::noBreak;
			}

			break;

		case LBC::cp:
			// [CP-$EastAsian] × (AL | HL | NU)
			if (!TextEditor::CodePoint::isEastAsian(state.current.codepoint) &&
				ALHLNU.find(state.next.cls) != ALHLNU.end()) {

				return TextEditor::BreakOption::noBreak;
			}

			break;

		default:
			return TextEditor::BreakOption::undefined;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb30a(LineBreakState& state) {
	// LB30a: break between two regional indicator symbols if and only if there are
	// an even number of regional indicators preceding the position of the break
	if (state.current.cls == LBC::ri) {
		if (state.next.cls == LBC::ri) {
			if (++state.ri % 2 != 0) {
				return TextEditor::BreakOption::noBreak;
			}
		}

	} else {
		state.ri = 0;
	}

	return TextEditor::BreakOption::undefined;
}


static inline TextEditor::BreakOption lb30b(LineBreakState& state) {
	// LB30b: do not break between an emoji base (or potential emoji) and an emoji modifier
	// EB × EM
	if ((state.current.cls == LBC::eb) && (state.next.cls == LBC::em)) {
		return TextEditor::BreakOption::noBreak;
	}

#if defined(IMGUI_USE_WCHAR32)
	// [\p{Extended_Pictographic}&\p{Cn}] × EM
	// if (state.next.cls == LBC::em && (/^\p{ExtPict}$/u.test(state.cur.char))  && (/^\p{gc=Cn}$/u.test(state.cur.char))) {
	// 	return TextEditor::BreakOption::noBreak;
	// }
#endif

	return TextEditor::BreakOption::undefined;
}


#define RULE(r) 													\
	result = r(state);												\
																	\
	if (result != TextEditor::BreakOption::undefined) {				\
		return result;												\
	}

#define RULE2(r) 													\
	if (config.r) {													\
		RULE(r)														\
	}


static inline TextEditor::BreakOption applyRules(TextEditor::LineBreakConfig config, LineBreakState& state) {
	TextEditor::BreakOption result;
	RULE2(lb2);
	RULE2(lb3);
	RULE2(lb4);
	RULE2(lb5);
	RULE2(lb6);
	RULE(lbSpaceStop);
	RULE2(lb7);
	RULE2(lb8);
	RULE2(lb8a);
	RULE2(lb9);
	RULE2(lb10);
	RULE2(lb11);
	RULE2(lb12);
	RULE2(lb12a);
	RULE2(lb13);
	RULE2(lb14);
	RULE2(lb15a);
	RULE2(lb15b);
	RULE2(lb15c);
	RULE2(lb15d);
	RULE2(lb16);
	RULE2(lb17);
	RULE2(lb18);
	RULE2(lb19);
	RULE2(lb19a);
	RULE2(lb20);
	RULE2(lb20a);
	RULE2(lb21a);
	RULE2(lb21);
	RULE2(lb21b);
	RULE2(lb22);
	RULE2(lb23);
	RULE2(lb23a);
	RULE2(lb24);
	RULE2(lb25);
	RULE2(lb26);
	RULE2(lb27);
	RULE2(lb28);
	RULE2(lb28a);
	RULE2(lb29);
	RULE2(lb30);
	RULE2(lb30a);
	RULE2(lb30b);

	// LB31: break everywhere else
	// ALL ÷
	// ÷ ALL
	return TextEditor::BreakOption::allowBreak;
}


//
//	TextEditor::LineBreak::classify
//

void TextEditor::LineBreak::classify(Line& line) {
	// handle easy cases
	auto size = line.size();

	if (size == 0) {
		return;

	} else if (size == 1) {
		line[0].breakOption = BreakOption::noBreak;
		return;
	}

	// handle different break modes
	if (config.useUnicodeAnnex14) {
		// setup state
		LineBreakState state;
		state.glyphs = &(*line.begin());
		state.size = size;

		for (size_t i = 0; i < size; i++) {
			auto& glyph = line[i];
			state.push(LineBreakGlyph(glyph.codepoint, getLineBreakClass(glyph.codepoint), i));
			auto breakOption = applyRules(config, state);

			if (i) {
				line[i - 1].breakOption = breakOption;
			}
		}

	} else {
		// update break sets (if required)
		if (updateSets) {
			updateSet(breakAfter, config.breakAfter);
			updateSet(breakBefore, config.breakBefore);
			updateSets = false;
		}

		// process all glyphs in line
		for (size_t i = 0; i < size; i++) {
			if (breakAfter.find(line[i].codepoint) != breakAfter.end()) {
				line[i].breakOption = BreakOption::allowBreak;

			} else if (i && breakBefore.find(line[i].codepoint) != breakBefore.end()) {
				line[i - 1].breakOption = BreakOption::allowBreak;
				line[i].breakOption = BreakOption::noBreak;

			} else {
				line[i].breakOption = BreakOption::noBreak;
			}
		}
	}

	line[size - 1].breakOption = BreakOption::noBreak;
}


//
//	TextEditor::LineBreak::updateSet
//

void TextEditor::LineBreak::updateSet(std::unordered_set<ImWchar>& set, std::string_view text) {
	// process UTF-8 and generate lines of glyphs
	set.clear();
	auto end = text.end();
	auto i = CodePoint::skipBOM(text.begin(), end);

	while (i < end) {
		ImWchar character;
		i = CodePoint::read(i, end, &character);
		set.insert(character);
	}
}
