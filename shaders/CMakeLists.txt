#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB SHADERS_INCLUDES *.h)
file(GLOB SHADERS_SOURCES *.cpp)
file(GLOB SHADERS_GLSL update */*.glsl */*.sc)

add_library(shaders OBJECT ${SHADERS_INCLUDES} ${SHADERS_SOURCES})
target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shaders PUBLIC language bgfx)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SHADERS_INCLUDES} ${SHADERS_SOURCES} ${SHADERS_GLSL})
