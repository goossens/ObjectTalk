//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#if _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#endif

#include <uv.h>

#include "OtException.h"


//
//	Error checking
//

#define UV_CHECK_ERROR(action, status) if (status < 0) OtError("Libuv error in %s: %s", action, uv_strerror((int) status))
#define UV_CHECK_ERROR2(action, status, details) if (status < 0) OtError("Libuv error in %s: %s [%s]", action, uv_strerror((int) status), details)


//
//	OtLibUvClass
//

class OtLibUv {
public:
	// initialize LibUV
	static void init(int argc, char* argv[]) {
		// setup the calling arguments
		uv_setup_args(argc, argv);
	}

	// run the libUV loop
	static void run() {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	// stop the libUV loop
	static void stop() {
		// use timer so "stop" transaction can complete
		static 	uv_timer_t uv_shutdown;
		uv_timer_init(uv_default_loop(), &uv_shutdown);

		uv_timer_start(&uv_shutdown, [](uv_timer_t* handle) {
			// close all handles which will end libuv's loop
			uv_walk(uv_default_loop(), [](uv_handle_t* handle, void* arg) {
				if (!uv_is_closing(handle)) {
					uv_close(handle, nullptr);
				}
			}, nullptr);
		}, 1000, 0);
	}

	// terminate LibUV
	static void end() {
		// properly close all libuv handles
		uv_walk(uv_default_loop(), [](uv_handle_t* handle, void* arg) {
			if (!uv_is_closing(handle))
				uv_close(handle, nullptr);
		}, nullptr);

		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
		uv_loop_close(uv_default_loop());

		// close the library
		uv_library_shutdown();
	}
};
