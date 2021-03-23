//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "ot/libuv.h"
#include "ot/exception.h"
#include "ot/main.h"


//
//	Globals
//

static bool valgrindMode = false;


//
//	OtInit
//

int OtInit(int argc, char* argv[]) {
	// see if flags we care about are set
	int newArgc = 1;

	for (auto i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--valgrind") {
			valgrindMode = true;

		} else {
			argv[newArgc++] = argv[i];
		}
	}
	// initialize libuv
	uv_setup_args(newArgc, argv);

	// return new argument count
	return newArgc;
}


//
//	OtEnd
//

void OtEnd() {
	// we only try to cleanup in valgrind mode
	if (valgrindMode) {
		OT_DEBUG("valgrindMode");
		// free all objects so we can detect true memory leaks

#if UV_VERSION_MINOR >= 38
		// free the last libuv memory
		uv_library_shutdown();
#endif
	}
}
