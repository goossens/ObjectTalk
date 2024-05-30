#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(ObjectTalk VERSION 1.0 DESCRIPTION "ObjectTalk - The Completely Object-Oriented Scripting Language" LANGUAGES C CXX)

if(APPLE)
	set(OT_GUI ON)
endif()

if(UNIX AND NOT APPLE)
	include(FindX11)
	set(OT_GUI ${X11_FOUND})
endif()

if(WIN32)
	set(OT_GUI ON)
endif()

add_compile_definitions("OT_DEBUG=$<CONFIG:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(deps)
add_subdirectory(3rdparty)
add_subdirectory(ot)
add_subdirectory(modules)

if(OT_GUI)
	add_subdirectory(shaders)
	add_subdirectory(gfx)
	add_subdirectory(gui)
	add_subdirectory(nodes)
	add_subdirectory(scene)
#	add_subdirectory(board)
	add_subdirectory(ide)
endif()

add_subdirectory(main)
add_subdirectory(examples)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_subdirectory(tests)

set_target_properties(examples tests PROPERTIES FOLDER other)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
