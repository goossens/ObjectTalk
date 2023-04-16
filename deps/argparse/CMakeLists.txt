#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(argparse GIT_REPOSITORY "https://github.com/p-ranav/argparse" GIT_TAG "v2.9")
FetchContent_GetProperties(argparse)

if(NOT argparse_POPULATED)
	FetchContent_Populate(argparse)
endif()

add_library(argparse INTERFACE)
target_include_directories(argparse INTERFACE ${argparse_SOURCE_DIR}/include)
