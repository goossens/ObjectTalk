#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(libuv GIT_REPOSITORY "https://github.com/libuv/libuv" GIT_TAG "v1.42.0")
SET(BUILD_TESTING OFF CACHE BOOL "Build tests." FORCE)
SET(LIBUV_BUILD_TESTS OFF CACHE BOOL "Build tests." FORCE)
SET(LIBUV_BUILD_BENCH OFF CACHE BOOL "Build benchmarks." FORCE)

FetchContent_GetProperties(libuv)

if(NOT libuv_POPULATED)
	FetchContent_Populate(libuv)
	add_subdirectory(${libuv_SOURCE_DIR} ${libuv_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${libuv_SOURCE_DIR}/include)

set_target_properties(uv uv_a PROPERTIES FOLDER deps)
