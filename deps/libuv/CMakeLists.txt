#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(LIBUV_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(LIBUV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LIBUV_BUILD_BENCH OFF CACHE BOOL "" FORCE)

# FetchContent_Declare(libuv GIT_REPOSITORY "https://github.com/libuv/libuv" GIT_TAG "v1.46.0")
FetchContent_Declare(libuv GIT_REPOSITORY "https://github.com/libuv/libuv" GIT_TAG "v1.x")
FetchContent_MakeAvailable(libuv)
set_target_properties(uv_a PROPERTIES FOLDER deps/libuv)
