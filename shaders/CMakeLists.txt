#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB SHADERS_INCLUDES *.h)
file(GLOB SHADERS_SOURCES update */*.vs */*.fs */*.sc)

add_library(shaders INTERFACE)
target_include_directories(shaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(shaders_src SOURCES ${SHADERS_INCLUDES} ${SHADERS_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SHADERS_INCLUDES} ${SHADERS_SOURCES})
