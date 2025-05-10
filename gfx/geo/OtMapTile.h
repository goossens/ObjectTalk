//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtHash.h"


//
//  OtMapTile
//

class OtMapTile {
public:
	// constructor
	OtMapTile(int z, int _x, int _y) : zoom(z), x(_x), y(_y) {
		hash = OtHash::generate(zoom, x, y);
	}

	// properties
	int zoom;
	int x, y;
	size_t hash;
};
