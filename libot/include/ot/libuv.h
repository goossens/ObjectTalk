//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif

#include <uv.h>

#include "ot/exception.h"


//
//	Error checking
//

#define UV_CHECK_ERROR(action, status) if (status < 0) OtExcept("Libuv error in %s: %s", action, uv_strerror(status))


//
//	OtLibUvClass
//

class OtLibUv {
public:
	// initialize LibUV
	static void init(int argc, char* argv[]) {
		uv_setup_args(argc, argv);
	}

	// terminate LibUV
	static void end() {
		uv_library_shutdown();
	}
};
