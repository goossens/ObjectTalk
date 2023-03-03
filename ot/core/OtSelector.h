//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtSelector
//

class OtSelector : public OtPerThreadSingleton<OtSelector> {
public:
	// get a new selector
	size_t get(const char* text) {
		return get(std::string_view(text));
	}

	size_t get(const std::string& text) {
		return get(std::string_view(text));
	}

	size_t get(const std::string_view text) {
		// see if this selector was already created
		if (selectorIndex.count(text)) {
			return selectorIndex[text];

		} else {
			auto selector = selectors.size();
			auto name = saveSelector(text);
			selectors.emplace_back(name);
			selectorIndex[name] = selector;
			return selector;
		}
	}

	// get the string associated with the selector
	const std::string_view get(size_t selector) {
		return selectors[selector];
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

	static std::string_view name(size_t selector) {
		return instance()->get(selector);
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

	// save selector name
	std::string_view saveSelector(const std::string_view text) {
		if (!buffers.size() || buffers.back().hasSpace(text.size())) {
			buffers.emplace_back();
		}

		return buffers.back().addChunk(text);
	}

	// properties
	std::list<Buffer> buffers;

	// list of selectors already created
	std::vector<std::string_view> selectors;
	std::unordered_map<std::string_view, size_t> selectorIndex;
};
