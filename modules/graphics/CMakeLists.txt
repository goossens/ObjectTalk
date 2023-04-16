#	ObjectTalk GRAPICS Module
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <graphicss://opensource.org/licenses/MIT>.

file(GLOB GRAPICS_INCLUDES *.h)
file(GLOB GRAPICS_SOURCES *.cpp)

set(GRAPICS_INCLUDE_DIRS "")

foreach(HEADER ${GRAPICS_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND GRAPICS_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES GRAPICS_INCLUDE_DIRS)

add_library(graphics STATIC ${GRAPICS_INCLUDES} ${GRAPICS_SOURCES})
add_dependencies(graphics math gfx ot glm)
target_include_directories(graphics PUBLIC ${GRAPICS_INCLUDE_DIRS})
target_link_libraries(graphics PUBLIC math gfx ot glm)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GRAPICS_INCLUDES} ${GRAPICS_SOURCES})
