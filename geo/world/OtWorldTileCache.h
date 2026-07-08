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

#include "OtLruCache.h"

#include "OtWorldTile.h"
#include "OtWorldTileID.h"


//
//	OtTileCache
//

template<typename T, size_t S=256>
class OtWorldTileCache : public OtLruCache<OtWorldTileID, OtWorldTile<T>, S> {
public:
	// constructor
	using Loader = std::function<void(OtWorldTile<T>&)>;
	OtWorldTileCache(const std::string& name, Loader loader) : name(name), loader(loader) {}

	OtWorldTile<T>& getOrCreate(const OtWorldTileID& id) {
		if (!this->has(id)) {
			set(id, OtWorldTile<T>(id));
			auto& tile = this->get(id);
			loader(tile);
			return tile;

		} else {
			return this->get(id);
		}
	}

private:
	// properties
	std::string name;
	Loader loader;
};
