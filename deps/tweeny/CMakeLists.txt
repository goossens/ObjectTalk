#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tweeny GIT_REPOSITORY "https://github.com/mobius3/tweeny")
FetchContent_GetProperties(tweeny)

if(NOT tweeny_POPULATED)
	FetchContent_Populate(tweeny)
endif()

add_library(tweeny INTERFACE)
target_include_directories(tweeny INTERFACE ${tweeny_SOURCE_DIR}/include)
