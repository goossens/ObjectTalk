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
#include <cstdint>
#include <cstring>
#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "OtSingleton.h"


//
//	OtID
//

using OtID = uint64_t;


//
//	OtIdentifier
//

class OtIdentifier : OtSingleton<OtIdentifier> {
public:
	// static shortcuts
	static inline OtID create(const char* text) {
		return instance().get(text);
	}

	static inline OtID create(const std::string& text) {
		return instance().get(text);
	}

	static inline OtID create(const std::string_view text) {
		return instance().get(text);
	}

	static inline std::string_view name(OtID id) {
		return instance().get(id);
	}

private:
	// get a new id
	inline OtID get(const char* text) {
		return get(std::string_view(text));
	}

	inline OtID get(const std::string& text) {
		return get(std::string_view(text));
	}

	inline OtID get(const std::string_view text) {
		// see if this id was already created
		if (identifierIndex.count(text)) {
			return identifierIndex[text];

		} else {
			OtID id = (OtID) identifiers.size();
			auto name = saveIdentifier(text);
			identifiers.emplace_back(name);
			identifierIndex[name] = id;
			return id;
		}
	}

	// get the string associated with the id
	inline const std::string_view get(OtID id) {
		return identifiers[id];
	}

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

	// save named id
	inline std::string_view saveIdentifier(const std::string_view text) {
		// create a new chunk buffer if required
		if (!buffers.size() || buffers.back().hasSpace(text.size())) {
			buffers.emplace_back();
		}

		// add chunk to buffer and return string view over new location
		return buffers.back().addChunk(text);
	}

	// properties
	std::list<Buffer> buffers;

	// list of identifiers already in use
	std::vector<std::string_view> identifiers;
	std::unordered_map<std::string_view, OtID> identifierIndex;
};
