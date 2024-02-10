//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	API definitions
//

#if __APPLE__
#define OT_GPU_METAL 1

#elif _WIN32
#define OT_GPU_DIRECT3D 1

#else
#define OT_GPU_VULKAN 1
#endif


//
//	Functions
//

// see if origin is in the bottom-left corner otherwise it's in the top-left corner
inline bool OtGpuHasOriginBottomLeft() {
	return bgfx::getCaps()->originBottomLeft;
}

// see if NDC depth is in [-1, 1] range, otherwise it's in the [0, 1] range
inline bool OtGpuHasHomogeneousDepth() {
	return bgfx::getCaps()->homogeneousDepth;
}
