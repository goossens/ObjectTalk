#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB NODES_INCLUDES CONFIGURE_DEPENDS */*.h)
file(GLOB NODES_SOURCES CONFIGURE_DEPENDS */*.cpp)

set(NODES_INCLUDE_DIRS "")

foreach(HEADER ${NODES_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND NODES_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES NODES_INCLUDE_DIRS)
set(NODES_INCLUDE_DIRS ${NODES_INCLUDE_DIRS} PARENT_SCOPE)

add_library(nodes OBJECT ${NODES_INCLUDES} ${NODES_SOURCES})

target_compile_options(nodes PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Werror -Wall -Wextra -Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Werror -Wall -Wextra -Wno-overloaded-virtual>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

target_include_directories(nodes PUBLIC ${NODES_INCLUDE_DIRS})
target_link_libraries(nodes PUBLIC language json gfx)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${NODES_INCLUDES} ${NODES_SOURCES})
