#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(bsthreadpool QUIET GIT_REPOSITORY "https://github.com/bshoshany/thread-pool" GIT_TAG "v5.0.0")

add_library(bsthreadpool INTERFACE)
target_include_directories(bsthreadpool INTERFACE ${bsthreadpool_SOURCE_DIR}/include)

file(GLOB BSTHREADPOOL_SOURCES ${bsthreadpool_SOURCE_DIR}/include/*.hpp)
add_custom_target(bsthreadpool_src SOURCES ${BSTHREADPOOL_SOURCES})
source_group(TREE "${bsthreadpool_SOURCE_DIR}/include" FILES ${BSTHREADPOOL_SOURCES})
set_target_properties(bsthreadpool_src PROPERTIES FOLDER deps)
