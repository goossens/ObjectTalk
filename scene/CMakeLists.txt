#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB SCENE_INCLUDES */*.h)
file(GLOB SCENE_SOURCES */*.cpp)

set(SCENE_INCLUDE_DIRS "")

foreach(HEADER ${SCENE_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND SCENE_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES SCENE_INCLUDE_DIRS)
set(SCENE_INCLUDE_DIRS ${SCENE_INCLUDE_DIRS} PARENT_SCOPE)

add_library(scene OBJECT ${SCENE_INCLUDES} ${SCENE_SOURCES})
target_include_directories(scene PUBLIC ${SCENE_INCLUDE_DIRS})

target_link_libraries(scene PUBLIC json gfx tweeny Jolt nodes)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SCENE_INCLUDES} ${SCENE_SOURCES})
