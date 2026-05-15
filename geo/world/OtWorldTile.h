//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtWorldTileID.h"


//
//	OtTile
//

template<typename T>
struct Landscape {
	// constructor
	Landscape(const OtTileID& id) : id(id) {}

	// check tile status
	inline bool isNull() { return asset.isNull(); }
	inline bool isLoading() { return asset.isLoading(); }
	inline bool isMissing() { return asset.isMissing(); }
	inline bool isInvalid() { return asset.isInvalid(); }
	inline bool isLoaded() { return asset.isLoaded(); }
	inline bool isReady() { return asset.isReady(); }
	inline bool isVirtual() { return asset.isVirtual(); }

	// properties
	OtTileID id;
	OtAsset<T> asset;
};
