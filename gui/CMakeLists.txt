#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB GUI_INCLUDES */*.h)
file(GLOB GUI_SOURCES */*.cpp)

set(GUI_INCLUDE_DIRS "")

foreach(HEADER ${GUI_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND GUI_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES GUI_INCLUDE_DIRS)
set(GUI_INCLUDE_DIRS ${GUI_INCLUDE_DIRS} PARENT_SCOPE)

add_library(gui OBJECT ${GUI_INCLUDES} ${GUI_SOURCES})
target_include_directories(gui PUBLIC ${GUI_INCLUDE_DIRS})

target_link_libraries(gui PUBLIC ot shaders gfx)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GUI_INCLUDES} ${GUI_SOURCES})
