#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB GEO_INCLUDES CONFIGURE_DEPENDS */*.h)
file(GLOB GEO_SOURCES CONFIGURE_DEPENDS */*.cpp)

set(GEO_INCLUDE_DIRS "")

foreach(HEADER ${GEO_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND GEO_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES GEO_INCLUDE_DIRS)
set(GEO_INCLUDE_DIRS ${GEO_INCLUDE_DIRS} PARENT_SCOPE)

add_library(geo OBJECT ${GEO_INCLUDES} ${GEO_SOURCES})

target_compile_options(geo PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Werror -Wall -Wextra>
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

target_include_directories(geo PUBLIC ${GEO_INCLUDE_DIRS})
target_link_libraries(geo PUBLIC json gfx)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GEO_INCLUDES} ${GEO_SOURCES})
