#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BOOST_REGEX_STANDALONE ON CACHE BOOL "" FORCE)

FetchContent_Declare(regex GIT_REPOSITORY "https://github.com/boostorg/regex" GIT_TAG "boost-1.83.0")
FetchContent_MakeAvailable(regex)

file(GLOB_RECURSE REGEX_SOURCES
	${regex_SOURCE_DIR}/include/boost/*.h
	${regex_SOURCE_DIR}/include/boost/*.hpp)

add_custom_target(regex_src SOURCES ${REGEX_SOURCES})
source_group(TREE "${regex_SOURCE_DIR}/include/boost" FILES ${REGEX_SOURCES})
set_target_properties(regex_src PROPERTIES FOLDER deps)
