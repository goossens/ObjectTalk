//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"


//
//	OtSymbol
//

struct OtSymbol {
	// constructors
	inline OtSymbol(size_t i, OtObject o, size_t os) : type(heapType), id(i), object(o), opcodeStart(os), opcodeEnd(0) {}
	inline OtSymbol(size_t i, size_t s, size_t os) : type(stackType), id(i), slot(s), opcodeStart(os), opcodeEnd(0) {}
	inline OtSymbol(size_t i, size_t os) : type(captureType), id(i), opcodeStart(os), opcodeEnd(0) {}

	enum Type {
		heapType,
		stackType,
		captureType
	};

	// properties
	Type type;
	size_t id;
	OtObject object;
	size_t slot;
	size_t opcodeStart;
	size_t opcodeEnd;
};
