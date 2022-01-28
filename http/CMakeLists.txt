#	ObjectTalk HTTP Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB HTTP_INCLUDES *.h)
file(GLOB HTTP_SOURCES *.cpp)

set(HTTP_INCLUDE_DIRS "")

foreach(HEADER ${HTTP_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND HTTP_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES HTTP_INCLUDE_DIRS)
set(HTTP_INCLUDE_DIRS ${HTTP_INCLUDE_DIRS} PARENT_SCOPE)

add_library(http STATIC ${HTTP_INCLUDES} ${HTTP_SOURCES})
add_dependencies(http ot)

target_include_directories(http PRIVATE ${OT_INCLUDE_DIRS} ${HTTP_INCLUDE_DIRS})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HTTP_INCLUDES} ${HTTP_SOURCES})
