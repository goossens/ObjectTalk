#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(LIBUV_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(LIBUV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LIBUV_BUILD_BENCH OFF CACHE BOOL "" FORCE)

# this hack uses a CMake internal property to stop generating test targets in IDEs
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

FetchContent_Populate(libuv QUIET GIT_REPOSITORY "https://github.com/libuv/libuv" GIT_TAG "v1.51.0")
add_subdirectory(${libuv_SOURCE_DIR} ${libuv_BINARY_DIR} EXCLUDE_FROM_ALL)

target_compile_options(uv_a PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wno-shorten-64-to-32 -Wno-gnu-folding-constant>
	$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wno-format-truncation>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/wd4334 /wd4244 /wd4245 /wd4267 /wd4701 /wd4702>)

if(CMAKE_GENERATOR STREQUAL "Xcode")
	set_target_properties(uv_a PROPERTIES XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO")
endif()

set_target_properties(uv_a PROPERTIES FOLDER deps/libuv)
