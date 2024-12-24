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
	// symbol types
	enum class Type {
		heap,
		stack,
		capture
	};

	// constructors
	inline OtSymbol(OtID i, OtObject o, size_t os) : type(Type::heap), id(i), object(o), opcodeStart(os), opcodeEnd(0) {}
	inline OtSymbol(OtID i, size_t s, size_t os) : type(Type::stack), id(i), slot(s), opcodeStart(os), opcodeEnd(0) {}
	inline OtSymbol(OtID i, size_t os) : type(Type::capture), id(i), opcodeStart(os), opcodeEnd(0) {}

	// properties
	Type type;
	OtID id;
	OtObject object;
	size_t slot;
	size_t opcodeStart;
	size_t opcodeEnd;
};
