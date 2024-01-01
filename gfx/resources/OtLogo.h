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

#include "OtTexture.h"


//
//	OtLogo
//

class OtLogo {
public:
	// get logo information
	uint16_t getTextureIndex();

	// get logo size
	inline size_t getWidth() { return 429; }
	inline size_t getHeight() { return 350; }

private:
	// the logo as a texture
	OtTexture texture;
};
