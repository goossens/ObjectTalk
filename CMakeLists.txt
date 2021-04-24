#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ObjectTalk VERSION 1.0 DESCRIPTION "ObjectTalk - The Completely Object-Oriented Scripting Language" LANGUAGES C CXX)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

include_directories(3rdparty)
include_directories(libot/include)

add_subdirectory(3rdparty)
add_subdirectory(libot)

add_subdirectory(app)

add_subdirectory(modules)
add_subdirectory(examples)
add_subdirectory(tests)

add_custom_target(other SOURCES LICENSE Makefile README.md)
