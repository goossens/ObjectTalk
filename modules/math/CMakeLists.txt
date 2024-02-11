#	ObjectTalk MATH Module
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <maths://opensource.org/licenses/MIT>.

file(GLOB MATH_INCLUDES *.h)
file(GLOB MATH_SOURCES *.cpp)

set(MATH_INCLUDE_DIRS "")

foreach(HEADER ${MATH_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND MATH_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES MATH_INCLUDE_DIRS)

add_library(math OBJECT ${MATH_INCLUDES} ${MATH_SOURCES})
add_dependencies(math ot glm)
target_include_directories(math PUBLIC ${MATH_INCLUDE_DIRS})
target_link_libraries(math PUBLIC ot glm)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${MATH_INCLUDES} ${MATH_SOURCES})
set_target_properties(math PROPERTIES FOLDER modules)
