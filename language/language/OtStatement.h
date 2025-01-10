//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtStatement
//

struct OtStatement {
	// constructor
	OtStatement(size_t ss, size_t se, size_t os, size_t oe) : sourceStart(ss), sourceEnd(se), opcodeStart(os), opcodeEnd(oe) {}

	// properties
	size_t sourceStart;
	size_t sourceEnd;
	size_t opcodeStart;
	size_t opcodeEnd;
};
