//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <cstring>
#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "OtSingleton.h"


//
//	OtSymbol
//

class OtSymbol : public OtSingleton<OtSymbol> {
public:
	// get a new symbol
	size_t get(const char* text) {
		return get(std::string_view(text));
	}

	size_t get(const std::string& text) {
		return get(std::string_view(text));
	}

	size_t get(const std::string_view text) {
		// see if this symbol was already created
		if (symbolIndex.count(text)) {
			return symbolIndex[text];

		} else {
			auto symbol = symbols.size();
			auto name = saveSymbol(text);
			symbols.emplace_back(name);
			symbolIndex[name] = symbol;
			return symbol;
		}
	}

	// get the string associated with the symbol
	const std::string_view get(size_t symbol) {
		return symbols[symbol];
	}

	// static shortcuts
	static size_t create(const char* text) {
		return instance()->get(text);
	}

	static size_t create(const std::string& text) {
		return instance()->get(text);
	}

	static size_t create(const std::string_view text) {
		return instance()->get(text);
	}

	static std::string_view name(size_t symbol) {
		return instance()->get(symbol);
	}

private:
	// buffer to store text chunks
	struct Buffer {
		// properties
		static constexpr size_t capacity = 4096;
		char buffer[capacity];
		size_t next = 0;

		// see if we have space for a specified number of characters
		bool hasSpace(size_t space) { return capacity - next > space; }

		// add a new chunk to the buffer and return pointer
		const std::string_view addChunk(const std::string_view text) {
			auto chunk = &buffer[next];
			auto size = text.size();
			std::memcpy(chunk, text.data(), size);
			next += size;
			return std::string_view(chunk, size);
		}
	};

	// save named symbol
	std::string_view saveSymbol(const std::string_view text) {
		// create a new chunk buffer if required
		if (!buffers.size() || buffers.back().hasSpace(text.size())) {
			buffers.emplace_back();
		}

		// add chunk to buffer and return string view over new location
		return buffers.back().addChunk(text);
	}

	// properties
	std::list<Buffer> buffers;

	// list of symbols already in use
	std::vector<std::string_view> symbols;
	std::unordered_map<std::string_view, size_t> symbolIndex;
};
