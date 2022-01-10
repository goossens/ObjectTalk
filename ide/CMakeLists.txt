#	ObjectTalk IDE Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE IDE_INCLUDES *.h)
file(GLOB_RECURSE IDE_SOURCES idemodule.cpp *.cpp)

add_library(ide STATIC ${IDE_INCLUDES} ${IDE_SOURCES})
add_dependencies(ide libot gui)
target_include_directories(ide PRIVATE ${GUI_INCLUDE_DIRS})

target_link_libraries(ide glfw bgfx imgui imguifd tinyobjloader box2d)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${IDE_INCLUDES} ${IDE_SOURCES})
