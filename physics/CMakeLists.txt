#	ObjectTalk PHYSICS Module
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB PHYSICS_INCLUDES */*.h)
file(GLOB PHYSICS_SOURCES */*.cpp)

set(PHYSICS_INCLUDE_DIRS "")

foreach(HEADER ${PHYSICS_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND PHYSICS_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES PHYSICS_INCLUDE_DIRS)

add_library(physics OBJECT ${PHYSICS_INCLUDES} ${PHYSICS_SOURCES})
target_include_directories(physics PUBLIC ${PHYSICS_INCLUDE_DIRS})
target_link_libraries(physics PUBLIC ot imgui box2d)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${PHYSICS_INCLUDES} ${PHYSICS_SOURCES})
set_target_properties(physics PROPERTIES FOLDER modules)
