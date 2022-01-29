#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(box2d GIT_REPOSITORY "https://github.com/erincatto/box2d" GIT_TAG "v2.4.1")
SET(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
SET(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)

FetchContent_GetProperties(box2d)

if(NOT box2d_POPULATED)
	FetchContent_Populate(box2d)
	add_subdirectory(${box2d_SOURCE_DIR} ${box2d_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${box2d_SOURCE_DIR}/include)

set_target_properties(box2d PROPERTIES FOLDER deps)
