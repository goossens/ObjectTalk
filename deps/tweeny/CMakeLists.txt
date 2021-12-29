#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tweeny GIT_REPOSITORY "https://github.com/mobius3/tweeny")

FetchContent_GetProperties(tweeny)

if(NOT tweeny_POPULATED)
	FetchContent_Populate(tweeny)
	add_subdirectory(${tweeny_SOURCE_DIR} ${tweeny_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${tweeny_SOURCE_DIR}/include)
