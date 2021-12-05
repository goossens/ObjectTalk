#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ObjectTalk VERSION 1.0 DESCRIPTION "ObjectTalk - The Completely Object-Oriented Scripting Language" LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug)
endif()

if(APPLE)
	set(OT_GUI_DEFAULT ON CACHE BOOL "Enable GUI module" FORCE)
else()
	set(OT_GUI_DEFAULT OFF CACHE BOOL "Enable GUI module" FORCE)
endif()

option(OT_GUI "Enable GUI module" ${OT_GUI_DEFAULT})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(deps/CMakeLists.txt)

include_directories(3rdparty)
add_subdirectory(3rdparty)

include_directories(BEFORE libot/include)
add_subdirectory(libot)

add_subdirectory(app)
add_subdirectory(modules)
add_subdirectory(examples)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_subdirectory(tests)

add_custom_target(other SOURCES LICENSE Makefile README.md)
