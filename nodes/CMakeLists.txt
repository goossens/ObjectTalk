#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB GRAPH_INCLUDES */*.h)
file(GLOB GRAPH_SOURCES */*.cpp)

set(GRAPH_INCLUDE_DIRS "")

foreach(HEADER ${GRAPH_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND GRAPH_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES GRAPH_INCLUDE_DIRS)
set(GRAPH_INCLUDE_DIRS ${GRAPH_INCLUDE_DIRS} PARENT_SCOPE)

add_library(nodes OBJECT ${GRAPH_INCLUDES} ${GRAPH_SOURCES})
target_include_directories(nodes PUBLIC ${GRAPH_INCLUDE_DIRS})

target_link_libraries(nodes PUBLIC ot json gfx)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GRAPH_INCLUDES} ${GRAPH_SOURCES})
