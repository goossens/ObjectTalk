//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>

#include "OtWorldTile.h"
#include "OtWorldTileID.h"


//
//	OtTileCache
//

template<typename T, size_t S=256>
class OtWorldTileCache : public OtLruCache<OtTileID, OtTile<T>, S> {
public:
	// constructor
	using Loader = std::function<void(OtTile<T>&)>;
	OtWorldTileCache(const std::string& name, Loader loader) : name(name), loader(loader) {}

	OtTile<T>& getOrCreate(const OtTileID& id) {
		if (!has(id)) {
			set(id, OtTile<T>(id));
			auto& tile = get(id);
			loader(tile);
			return tile;

		} else {
			return get(id);
		}
	}

private:
	// properties
	std::string name;
	Loader loader;
};
