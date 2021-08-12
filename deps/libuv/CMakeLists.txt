#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

find_path(UV_INCLUDES uv.h)

if(UV_INCLUDES STREQUAL "")
	FetchContent_Declare(libuv GIT_REPOSITORY "https://github.com/libuv/libuv" GIT_TAG "v1.41.1")
	SET(BUILD_TESTING OFF CACHE BOOL "Build tests." FORCE)
	SET(LIBUV_BUILD_TESTS OFF CACHE BOOL "Build tests." FORCE)
	SET(LIBUV_BUILD_BENCH OFF CACHE BOOL "Build benchmarks." FORCE)
	FetchContent_MakeAvailable(libuv)
	include_directories(${libuv_SOURCE_DIR}/include)
	set(UV_LIBRARY uv)
	set(PRIVATE_UV ON)

else()
	include_directories(${UV_INCLUDES})
	find_library(UV_LIBRARY uv)
	set(PRIVATE_UV OFF)
endif()
