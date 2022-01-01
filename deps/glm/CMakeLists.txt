#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_TAG "0.9.9.8")

FetchContent_GetProperties(glm)

if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${glm_SOURCE_DIR})
