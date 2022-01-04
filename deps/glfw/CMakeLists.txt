#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(glfw GIT_REPOSITORY "https://github.com/glfw/glfw" GIT_TAG "3.3.6")
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_GetProperties(glfw)

if(NOT glfw_POPULATED)
	FetchContent_Populate(glfw)
	add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${glfw_SOURCE_DIR}/include)

set_target_properties(glfw update_mappings PROPERTIES FOLDER deps)
