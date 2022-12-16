//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	Functions

// see if origin is in bottom left corner
inline bool OtGpuHasOriginBottomLeft() {
	return bgfx::getCaps()->originBottomLeft;
}

// see if Z coordinates are homogeneous (-1.0 to 1.0)
inline bool OtGpuHasHomogeneousDepth() {
	return bgfx::getCaps()->homogeneousDepth;
}
