#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB OT_INCLUDES */*.h)
file(GLOB OT_SOURCES */*.cpp)

set(OT_INCLUDE_DIRS "")

foreach(HEADER ${OT_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND OT_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES OT_INCLUDE_DIRS)

add_library(ot OBJECT ${OT_INCLUDES} ${OT_SOURCES})
add_dependencies(ot uv_a json fmt)

if (UNIX)
	set(OT_EXTRA_LIBS m)
endif()

target_include_directories(ot PUBLIC ${OT_INCLUDE_DIRS})
target_link_libraries(ot PUBLIC uv_a json fmt ${OT_EXTRA_LIBS})

set_target_properties(ot PROPERTIES PREFIX "")
set_target_properties(ot PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(ot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set_target_properties(ot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${OT_INCLUDES} ${OT_SOURCES})
