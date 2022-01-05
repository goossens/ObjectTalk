#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(bgfx GIT_REPOSITORY "https://github.com/bkaradzic/bgfx.cmake")
SET(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
SET(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_CONFIG_DEBUG OFF CACHE INTERNAL "" FORCE)
set(BGFX_INSTALL OFF CACHE INTERNAL "" FORCE)

FetchContent_GetProperties(bgfx)

if(NOT bgfx_POPULATED)
	FetchContent_Populate(bgfx)
	add_subdirectory(${bgfx_SOURCE_DIR} ${bgfx_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${bgfx_SOURCE_DIR}/bx/include)
include_directories(${bgfx_SOURCE_DIR}/bimg/include)

if (WIN32)
include_directories(${bgfx_SOURCE_DIR}/bx/include/compat/msvc)
endif()

set_target_properties(bx bimg bgfx PROPERTIES FOLDER deps)
set_target_properties(astc astc-codec edtaa3 etc1 etc2 iqa squish nvtt pvrtc tinyexr PROPERTIES FOLDER deps/bimg)
