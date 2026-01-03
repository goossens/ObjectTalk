#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(box2d GIT_REPOSITORY "https://github.com/erincatto/box2d" GIT_TAG "v2.4.1")
FetchContent_MakeAvailable(box2d)

if(CMAKE_GENERATOR STREQUAL "Xcode")
	set_target_properties(box2d PROPERTIES XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO")
endif()

set_target_properties(box2d PROPERTIES FOLDER deps)
