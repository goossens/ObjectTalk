#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(argparse QUIET GIT_REPOSITORY "https://github.com/p-ranav/argparse" GIT_TAG "v3.2")

add_library(argparse INTERFACE)
target_include_directories(argparse INTERFACE ${argparse_SOURCE_DIR}/include)

file(GLOB ARGPARSE_SOURCES ${argparse_SOURCE_DIR}/include/argparse/*.hpp)
add_custom_target(argparse_src SOURCES ${ARGPARSE_SOURCES})
source_group(TREE "${argparse_SOURCE_DIR}/include" FILES ${ARGPARSE_SOURCES})
set_target_properties(argparse_src PROPERTIES FOLDER deps)
