#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(lunasvg QUIET GIT_REPOSITORY "https://github.com/sammycage/lunasvg" GIT_TAG "v3.3.0")

file(GLOB LUNASVG_SOURCES
	${lunasvg_SOURCE_DIR}/include/*.h
	${lunasvg_SOURCE_DIR}/source/*.h
	${lunasvg_SOURCE_DIR}/source/*.cpp)

add_library(lunasvg STATIC ${LUNASVG_SOURCES})
target_compile_definitions(lunasvg PUBLIC LUNASVG_BUILD_STATIC)
target_include_directories(lunasvg PUBLIC ${lunasvg_SOURCE_DIR}/include)
target_link_libraries(lunasvg PUBLIC plutovg)

set_target_properties(lunasvg PROPERTIES FOLDER "deps")
