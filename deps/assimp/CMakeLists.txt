#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp" GIT_TAG "v5.0.1")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)

FetchContent_GetProperties(assimp)

if(NOT assimp_POPULATED)
	FetchContent_Populate(assimp)
	add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${assimp_SOURCE_DIR}/include)
