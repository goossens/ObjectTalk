//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/codepoint.h"
#include "ot/exception.h"


//
//	OtCodePointSize
//

size_t OtCodePoint::size(std::string::const_iterator i) {
	if ((*i & 0x80) == 0) {
		return 1;

	} else if ((*i & 0xE0) == 0xC0) {
		return 2;

	} else if ((*i & 0xF0) == 0xE0) {
		return 3;

	} else if ((*i & 0xF8) == 0xF0) {
		return 4;

	} else {
		OtExcept("Invalid codepoint in UTF-8 string");
	}

	return 0;
}


//
//	OtCodePointGet
//

std::string::const_iterator OtCodePoint::get(std::string::const_iterator i, int32_t* codeword) {
	if ((*i & 0x80) == 0) {
		*codeword = *i;
		i++;

	} else if ((*i & 0xE0) == 0xC0) {
		*codeword = ((*i & 0x1f) << 6) | (*(i + 1) & 0x3f);
		i += 2;

	} else if ((*i & 0xF0) == 0xE0) {
		*codeword = ((*i & 0x0f) << 12) | ((*(i + 1) & 0x3f) << 6) | (*(i + 2) & 0x3f);
		i += 3;

	} else if ((*i & 0xF8) == 0xF0) {
		*codeword = ((*i & 0x07) << 18) | ((*(i + 1) & 0x3f) << 12) | ((*(i + 2) & 0x3f) << 6) | (*(i + 3) & 0x3f);
		i += 4;

	} else {
		OtExcept("Invalid codepoint in UTF-8 string");
	}

	return i;
}


//
//	OtCodePointPut
//

std::string::iterator OtCodePoint::put(std::string::iterator i, int32_t codeword) {
	if ((codeword & 0xffffff80) == 0) {
		*i = codeword;
		i++;

	} else if ((codeword & 0xffffff80) == 0) {
		*i = 0xc0 | ((codeword >> 6) & 0x1f);
		*(i + 1) = 0x80 | (codeword & 0x3f);
		i += 2;

	} else if ((codeword & 0xffff0000) == 0) {
		*i = 0xe0 | ((codeword >> 12) & 0x0f);
		*(i + 1) = 0x80 | ((codeword >> 6) & 0x3f);
		*(i + 2) = 0x80 | (codeword & 0x3f);
		i += 3;

	} else {
		*i = 0xf0 | ((codeword >> 18) & 0x07);
		*(i + 1) = 0x80 | ((codeword >> 12) & 0x3f);
		*(i + 2) = 0x80 | ((codeword >> 6) & 0x3f);
		*(i + 3) = 0x80 | (codeword & 0x3f);
		i += 4;
	}

	return i;
}


//
//	OtCodePointLower
//

int32_t OtCodePoint::lower(int32_t cp) {
	if ((cp >= 0x0041 && cp <= 0x005a) ||
		(cp >= 0x00c0 && cp <= 0x00d6) ||
		(cp >= 0x00d8 && cp <= 0x00de) ||
		(cp >= 0x0391 && cp <= 0x03a1) ||
		(cp >= 0x03a3 && cp <= 0x03ab) ||
		(cp >= 0x0410 && cp <= 0x042f)) {
			cp += 32;

	} else if (cp >= 0x0400 && cp <= 0x040f) {
		cp += 80;

	} else if ((cp >= 0x0100 && cp <= 0x012f) ||
		(cp >= 0x0132 && cp <= 0x0137) ||
		(cp >= 0x014a && cp <= 0x0177) ||
		(cp >= 0x0182 && cp <= 0x0185) ||
		(cp >= 0x01a0 && cp <= 0x01a5) ||
		(cp >= 0x01de && cp <= 0x01ef) ||
		(cp >= 0x01f8 && cp <= 0x021f) ||
		(cp >= 0x0222 && cp <= 0x0233) ||
		(cp >= 0x0246 && cp <= 0x024f) ||
		(cp >= 0x03d8 && cp <= 0x03ef) ||
		(cp >= 0x0460 && cp <= 0x0481) ||
		(cp >= 0x048a && cp <= 0x04ff)) {
			cp |= 0x1;

	} else if ((cp >= 0x0139 && cp <= 0x0148) ||
		(cp >= 0x0179 && cp <= 0x017e) ||
		(cp >= 0x01af && cp <= 0x01b0) ||
		(cp >= 0x01b3 && cp <= 0x01b6) ||
		(cp >= 0x01cd && cp <= 0x01dc)) {
			cp += 1;
			cp &= ~0x1;

	} else {
		switch (cp) {
			case 0x0178: cp = 0x00ff; break;
			case 0x0243: cp = 0x0180; break;
			case 0x018e: cp = 0x01dd; break;
			case 0x023d: cp = 0x019a; break;
			case 0x0220: cp = 0x019e; break;
			case 0x01b7: cp = 0x0292; break;
			case 0x01c4: cp = 0x01c6; break;
			case 0x01c7: cp = 0x01c9; break;
			case 0x01ca: cp = 0x01cc; break;
			case 0x01f1: cp = 0x01f3; break;
			case 0x01f7: cp = 0x01bf; break;
			case 0x0187: cp = 0x0188; break;
			case 0x018b: cp = 0x018c; break;
			case 0x0191: cp = 0x0192; break;
			case 0x0198: cp = 0x0199; break;
			case 0x01a7: cp = 0x01a8; break;
			case 0x01ac: cp = 0x01ad; break;
			case 0x01af: cp = 0x01b0; break;
			case 0x01b8: cp = 0x01b9; break;
			case 0x01bc: cp = 0x01bd; break;
			case 0x01f4: cp = 0x01f5; break;
			case 0x023b: cp = 0x023c; break;
			case 0x0241: cp = 0x0242; break;
			case 0x03fd: cp = 0x037b; break;
			case 0x03fe: cp = 0x037c; break;
			case 0x03ff: cp = 0x037d; break;
			case 0x037f: cp = 0x03f3; break;
			case 0x0386: cp = 0x03ac; break;
			case 0x0388: cp = 0x03ad; break;
			case 0x0389: cp = 0x03ae; break;
			case 0x038a: cp = 0x03af; break;
			case 0x038c: cp = 0x03cc; break;
			case 0x038e: cp = 0x03cd; break;
			case 0x038f: cp = 0x03ce; break;
			case 0x0370: cp = 0x0371; break;
			case 0x0372: cp = 0x0373; break;
			case 0x0376: cp = 0x0377; break;
			case 0x03f4: cp = 0x03b8; break;
			case 0x03cf: cp = 0x03d7; break;
			case 0x03f9: cp = 0x03f2; break;
			case 0x03f7: cp = 0x03f8; break;
			case 0x03fa: cp = 0x03fb; break;
			default: break;
		}
	}

	return cp;
}


//
//	OtCodePointUpper
//

int32_t OtCodePoint::upper(int32_t cp) {
	if ((cp >= 0x0061 && cp <= 0x007a) ||
		(cp >= 0x00e0 && cp <= 0x00f6) ||
		(cp >= 0x00f8 && cp <= 0x00fe) ||
		(cp >= 0x03b1 && cp <= 0x03c1) ||
		(cp >= 0x03c3 && cp <= 0x03cb) ||
		(cp >= 0x0430 && cp <= 0x044f)) {
			cp -= 32;

	} else if (cp >= 0x0450 && cp <= 0x045f) {
		cp -= 80;

	} else if ((cp >= 0x0100 && cp <= 0x012f) ||
		(cp >= 0x0132 && cp <= 0x0137) ||
		(cp >= 0x014a && cp <= 0x0177) ||
		(cp >= 0x0182 && cp <= 0x0185) ||
		(cp >= 0x01a0 && cp <= 0x01a5) ||
		(cp >= 0x01de && cp <= 0x01ef) ||
		(cp >= 0x01f8 && cp <= 0x021f) ||
		(cp >= 0x0222 && cp <= 0x0233) ||
		(cp >= 0x0246 && cp <= 0x024f) ||
		(cp >= 0x03d8 && cp <= 0x03ef) ||
		(cp >= 0x0460 && cp <= 0x0481) ||
		(cp >= 0x048a && cp <= 0x04ff)) {
			cp &= ~0x1;

	} else if ((cp >= 0x0139 && cp <= 0x0148) ||
		(cp >= 0x0179 && cp <= 0x017e) ||
		(cp >= 0x01af && cp <= 0x01b0) ||
		(cp >= 0x01b3 && cp <= 0x01b6) ||
		(cp >= 0x01cd && cp <= 0x01dc)) {
			cp -= 1;
			cp |= 0x1;

	} else {
		switch (cp) {
			case 0x00ff: cp = 0x0178; break;
			case 0x0180: cp = 0x0243; break;
			case 0x01dd: cp = 0x018e; break;
			case 0x019a: cp = 0x023d; break;
			case 0x019e: cp = 0x0220; break;
			case 0x0292: cp = 0x01b7; break;
			case 0x01c6: cp = 0x01c4; break;
			case 0x01c9: cp = 0x01c7; break;
			case 0x01cc: cp = 0x01ca; break;
			case 0x01f3: cp = 0x01f1; break;
			case 0x01bf: cp = 0x01f7; break;
			case 0x0188: cp = 0x0187; break;
			case 0x018c: cp = 0x018b; break;
			case 0x0192: cp = 0x0191; break;
			case 0x0199: cp = 0x0198; break;
			case 0x01a8: cp = 0x01a7; break;
			case 0x01ad: cp = 0x01ac; break;
			case 0x01b0: cp = 0x01af; break;
			case 0x01b9: cp = 0x01b8; break;
			case 0x01bd: cp = 0x01bc; break;
			case 0x01f5: cp = 0x01f4; break;
			case 0x023c: cp = 0x023b; break;
			case 0x0242: cp = 0x0241; break;
			case 0x037b: cp = 0x03fd; break;
			case 0x037c: cp = 0x03fe; break;
			case 0x037d: cp = 0x03ff; break;
			case 0x03f3: cp = 0x037f; break;
			case 0x03ac: cp = 0x0386; break;
			case 0x03ad: cp = 0x0388; break;
			case 0x03ae: cp = 0x0389; break;
			case 0x03af: cp = 0x038a; break;
			case 0x03cc: cp = 0x038c; break;
			case 0x03cd: cp = 0x038e; break;
			case 0x03ce: cp = 0x038f; break;
			case 0x0371: cp = 0x0370; break;
			case 0x0373: cp = 0x0372; break;
			case 0x0377: cp = 0x0376; break;
			case 0x03d1: cp = 0x0398; break;
			case 0x03d7: cp = 0x03cf; break;
			case 0x03f2: cp = 0x03f9; break;
			case 0x03f8: cp = 0x03f7; break;
			case 0x03fb: cp = 0x03fa; break;
			default: break;
		}
	}

	return cp;
}
