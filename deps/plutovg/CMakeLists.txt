#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(plutovg QUIET GIT_REPOSITORY "https://github.com/sammycage/plutovg" GIT_TAG "v0.0.13")

file(GLOB PLUTOVG_SOURCES
	${plutovg_SOURCE_DIR}/include/*.h
	${plutovg_SOURCE_DIR}/source/*.c)

add_library(plutovg STATIC ${PLUTOVG_SOURCES})
target_compile_definitions(plutovg PUBLIC "PLUTOVG_BUILD_STATIC")
target_include_directories(plutovg PUBLIC ${plutovg_SOURCE_DIR}/include)

set_target_properties(plutovg PROPERTIES FOLDER "deps")
