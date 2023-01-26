#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB STAGE_INCLUDES */*.h)
file(GLOB STAGE_SOURCES */*.cpp)

set(STAGE_INCLUDE_DIRS "")

foreach(HEADER ${STAGE_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND STAGE_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES STAGE_INCLUDE_DIRS)
set(STAGE_INCLUDE_DIRS ${STAGE_INCLUDE_DIRS} PARENT_SCOPE)

add_library(stage STATIC ${STAGE_INCLUDES} ${STAGE_SOURCES})
target_include_directories(stage PUBLIC ${STAGE_INCLUDE_DIRS})

add_dependencies(stage
	gui
#	tinygltf
	tinyobjloader)

target_link_libraries(stage PUBLIC
	gui
#	tinygltf
	tinyobjloader)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${STAGE_INCLUDES} ${STAGE_SOURCES})
