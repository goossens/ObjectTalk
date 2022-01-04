#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(ObjectTalk VERSION 1.0 DESCRIPTION "ObjectTalk - The Completely Object-Oriented Scripting Language" LANGUAGES C CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include("gui")
getGuiDefault()
option(OT_GUI "Enable GUI module" ${OT_GUI_DEFAULT})

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(deps/CMakeLists.txt)

include_directories(3rdparty)
add_subdirectory(3rdparty)

include_directories(BEFORE libot/include)
add_subdirectory(libot)

if(OT_GUI)
	include_directories(BEFORE gui)
	add_subdirectory(gui)
endif()

add_subdirectory(app)
add_subdirectory(examples)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_subdirectory(tests)

set_target_properties(examples tests PROPERTIES FOLDER other)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ot)
