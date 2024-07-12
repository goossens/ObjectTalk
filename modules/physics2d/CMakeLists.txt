#	ObjectTalk PHYSICS2D Module
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <physics2ds://opensource.org/licenses/MIT>.

file(GLOB PHYSICS2D_INCLUDES *.h)
file(GLOB PHYSICS2D_SOURCES *.cpp)

set(PHYSICS2D_INCLUDE_DIRS "")

foreach(HEADER ${PHYSICS2D_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND PHYSICS2D_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES PHYSICS2D_INCLUDE_DIRS)

add_library(physics2d OBJECT ${PHYSICS2D_INCLUDES} ${PHYSICS2D_SOURCES})
add_dependencies(physics2d ot box2d)
target_include_directories(physics2d PUBLIC ${PHYSICS2D_INCLUDE_DIRS})
target_link_libraries(physics2d PUBLIC ot box2d)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${PHYSICS2D_INCLUDES} ${PHYSICS2D_SOURCES})
set_target_properties(physics2d PROPERTIES FOLDER modules)
