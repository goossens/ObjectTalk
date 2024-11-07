#	ObjectTalk IDE Module
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB IDE_INCLUDES */*.h)
file(GLOB IDE_SOURCES */*.cpp)

set(IDE_INCLUDE_DIRS "")

foreach(HEADER ${IDE_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND IDE_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES IDE_INCLUDE_DIRS)

add_library(ide OBJECT ${IDE_INCLUDES} ${IDE_SOURCES})
target_include_directories(ide PUBLIC ${IDE_INCLUDE_DIRS})
target_link_libraries(ide PUBLIC language gfx nodes scene imguieditor imguizmo)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${IDE_INCLUDES} ${IDE_SOURCES})
