#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(httplib QUIET GIT_REPOSITORY "https://github.com/yhirose/cpp-httplib" GIT_TAG "v0.20.1")

add_library(httplib INTERFACE)
target_include_directories(httplib INTERFACE ${httplib_SOURCE_DIR})

file(GLOB HTTPLIB_SOURCES ${httplib_SOURCE_DIR}/httplib.h)
add_custom_target(httplib_src SOURCES ${httplib_SOURCE_DIR})
source_group(TREE "${httplib_SOURCE_DIR}" FILES ${HTTPLIB_SOURCES})
set_target_properties(httplib_src PROPERTIES FOLDER deps)
