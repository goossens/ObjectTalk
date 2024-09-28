//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <vector>

#include "OtSymbol.h"


//
//	OtSymbolTable
//

struct OtSymbolTable {
	// declare a new symbol located on the heap
	inline void declare(size_t id, OtObject object, size_t opcodeStart) {
		symbols.emplace_back(id, object, opcodeStart, true);
	}

	// declare a new symbol located on the stack
	inline void declare(size_t id, size_t slot, size_t opcodeStart) {
		symbols.emplace_back(id, slot, opcodeStart, true);
	}

	// reference a symbol located on the heap
	inline void reference(size_t id, OtObject object, size_t opcodeStart) {
		symbols.emplace_back(id, object, opcodeStart, false);
	}

	// reference a symbol located on the stack
	inline void reference(size_t id, size_t slot, size_t opcodeStart) {
		symbols.emplace_back(id, slot, opcodeStart, false);
	}
	// end visibility of specified symbol
	inline void hide(size_t id, size_t opcodeEnd) {
		lookup(id)->opcodeEnd = opcodeEnd;
	}

	// see if symbol is known
	inline bool has(size_t id) {
		return lookup(id) != symbols.rend();
	}

	// see if symbol is declared
	inline bool isDeclared(size_t id) {
		auto i = lookup(id);
		return i != symbols.rend() && i->declared;
	}

	// lookup a symbol
	inline std::vector<OtSymbol>::reverse_iterator lookup(size_t id) {
		return std::find_if(symbols.rbegin(), symbols.rend(), [id](const OtSymbol& symbol){
			return symbol.id == id && !symbol.opcodeEnd;
		});
	}

	std::vector<OtSymbol> symbols;
};
