//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtObject.h"


//
//	OtSymbolTable
//

class OtSymbolTable {
public:
	// add symbol table entries
	inline void addHeapObject(size_t symbol, OtObject object) { entries.emplace_back(symbol, object); }
	inline void addStackItem(size_t symbol , size_t slot) { entries.emplace_back(symbol, slot); }

private:
	// symbol table entries
	enum Type {
		heapEntry,
		stackEntry
	};

	struct Entry {
		// constructors
		Entry(size_t s, OtObject o) : type(heapEntry), symbol(s), object(o) {}
		Entry(size_t s, size_t o) : type(stackEntry), symbol(s), slot(o) {}

		// properties
		Type type;
		size_t symbol;
		OtObject object;
		size_t slot;
	};

	std::vector<Entry> entries;
};
