#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(LIBUV_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(LIBUV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LIBUV_BUILD_BENCH OFF CACHE BOOL "" FORCE)

# this hack uses a CMake internal property to stop generating test targets in IDEs
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

FetchContent_GetProperties(libuv)

if(NOT libuv_POPULATED)
	FetchContent_Populate(libuv QUIET GIT_REPOSITORY "https://github.com/libuv/libuv" GIT_TAG "v1.48.0")
	add_subdirectory(${libuv_SOURCE_DIR} ${libuv_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set_target_properties(uv_a PROPERTIES FOLDER deps/libuv)
