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
	inline OtSymbol(size_t i, OtObject o, size_t os, bool d) : type(heapType), id(i), object(o), opcodeStart(os), opcodeEnd(0), declared(d) {}
	inline OtSymbol(size_t i, size_t s, size_t os, bool d) : type(stackType), id(i), slot(s), opcodeStart(os), opcodeEnd(0), declared(d) {}

	enum Type {
		heapType,
		stackType
	};

	// properties
	Type type;
	size_t id;
	OtObject object;
	size_t slot;
	size_t opcodeStart;
	size_t opcodeEnd;
	bool declared;
};