#	ObjectTalk INPUT Module
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <inputs://opensource.org/licenses/MIT>.

file(GLOB INPUT_INCLUDES *.h)
file(GLOB INPUT_SOURCES *.cpp)

set(INPUT_INCLUDE_DIRS "")

foreach(HEADER ${INPUT_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND INPUT_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES INPUT_INCLUDE_DIRS)

add_library(input OBJECT ${INPUT_INCLUDES} ${INPUT_SOURCES})
add_dependencies(input ot imgui math)
target_include_directories(input PUBLIC ${INPUT_INCLUDE_DIRS})
target_link_libraries(input PUBLIC ot imgui math)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${INPUT_INCLUDES} ${INPUT_SOURCES})
set_target_properties(input PROPERTIES FOLDER modules)
