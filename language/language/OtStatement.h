//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>


//
//	OtStatement
//

struct OtStatement {
	// constructor
	OtStatement(size_t sourceStart, size_t sourceEnd, size_t opcodeStart, size_t opcodeEnd) :
		sourceStart(sourceStart), sourceEnd(sourceEnd), opcodeStart(opcodeStart), opcodeEnd(opcodeEnd) {}

	// properties
	size_t sourceStart;
	size_t sourceEnd;
	size_t opcodeStart;
	size_t opcodeEnd;
};
