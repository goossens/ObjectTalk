#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB GUI_INCLUDES */*.h)
file(GLOB GUI_SOURCES */*.cpp)

if (APPLE)
	# for MacOS
	list(APPEND GUI_SOURCES framework/OtMacos.mm)

	find_library(COCOA Cocoa REQUIRED)
	find_library(QUARTZCORE QuartzCore REQUIRED)
	find_library(METAL Metal REQUIRED)
	find_library(METALKIT MetalKit REQUIRED)

	set(GUI_LIBS ${COCOA} ${QUARTZCORE} ${METAL} ${METALKIT})
endif()

set(GUI_INCLUDE_DIRS "")

foreach(HEADER ${GUI_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND GUI_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES GUI_INCLUDE_DIRS)
set(GUI_INCLUDE_DIRS ${GUI_INCLUDE_DIRS} PARENT_SCOPE)

add_library(gui STATIC ${GUI_INCLUDES} ${GUI_SOURCES})
add_dependencies(gui ot glm glfw bgfx tess2 imgui imguifd imguieditor tinyobjloader box2d tweeny)
target_include_directories(gui PUBLIC ${GUI_INCLUDE_DIRS})
target_link_libraries(gui PUBLIC ot glm glfw bgfx tess2 imgui imguifd imguieditor tinyobjloader box2d tweeny ${GUI_LIBS})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GUI_INCLUDES} ${GUI_SOURCES})
