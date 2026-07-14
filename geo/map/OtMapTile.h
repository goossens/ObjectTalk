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
//	OtMapTile
//

class OtMapTile {
public:
	// constructor
	OtMapTile(int z, int x, int y) : zoom(z), x(x), y(y) {
		hash = OtHash::generate(zoom, x, y);
	}

	// properties
	int zoom;
	int x, y;
	size_t hash;
};
