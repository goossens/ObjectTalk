//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtHash.h"


//
//	OtWorldTileID
//

class OtWorldTileID {
public:
	// constructors
	OtWorldTileID() = default;
	OtWorldTileID(size_t level, size_t tx, size_t ty) : level(level), tx(tx), ty(ty) {}

	// C++20 generates operator== automatically
	bool operator==(const OtWorldTileID&) const = default;

	// get hash value
	inline size_t hash() const { return OtHash::generate(level, tx, ty); }

	// properties
	size_t level = 0;
	size_t tx = 0;
	size_t ty = 0;
};


//
//	specialized std::hash for OtTileID
//

namespace std {
	template<>
	struct hash<OtTileID> {
		size_t operator()(const OtTileID& id) const noexcept {
			// combine hashes of members
			return id.hash();
		}
	};
}
