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
	inline OtSymbol(size_t s, OtObject o) : type(heapType), id(s), object(o) {}
	inline OtSymbol(size_t s, size_t o) : type(stackType), id(s), slot(o) {}

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
	bool referenced;
};
