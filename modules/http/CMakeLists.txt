#	ObjectTalk HTTP Module
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

add_library(http OBJECT ${HTTP_INCLUDES} ${HTTP_SOURCES})
add_dependencies(http ot llhttp multipart)
target_include_directories(http PUBLIC ${HTTP_INCLUDE_DIRS})
target_link_libraries(http PUBLIC ot PRIVATE llhttp multipart)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HTTP_INCLUDES} ${HTTP_SOURCES})
set_target_properties(http PROPERTIES FOLDER modules)
