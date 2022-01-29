#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(deps/CMakeLists.txt)

include_directories(3rdparty)
add_subdirectory(3rdparty)

add_subdirectory(ot)
include_directories(BEFORE ${OT_INCLUDE_DIRS})

add_subdirectory(http)
include_directories(BEFORE ${HTTP_INCLUDE_DIRS})

if(OT_GUI)
	add_subdirectory(gui)
	include_directories(BEFORE ${GUI_INCLUDE_DIRS})
	add_subdirectory(ide)
	include_directories(BEFORE ${IDE_INCLUDE_DIRS})
endif()

add_subdirectory(main)
add_subdirectory(examples)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_subdirectory(tests)

set_target_properties(examples tests PROPERTIES FOLDER other)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
