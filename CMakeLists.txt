#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(ObjectTalk VERSION 1.0 DESCRIPTION "ObjectTalk - The Completely Object-Oriented Scripting Language" LANGUAGES C CXX)

if(APPLE)
	set(OT_INCLUDE_UI ON)
endif()

if(UNIX AND NOT APPLE)
	include(FindX11)
	set(OT_INCLUDE_UI ${X11_FOUND})
endif()

if(WIN32)
	set(OT_INCLUDE_UI ON)
endif()

add_compile_definitions("OT_DEBUG=$<CONFIG:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(deps)
add_subdirectory(3rdparty)
add_subdirectory(language)
add_subdirectory(network)

if(OT_INCLUDE_UI)
	add_subdirectory(audio)
	add_subdirectory(shaders)
	add_subdirectory(gfx)
	add_subdirectory(gui)
	add_subdirectory(nodes)
	add_subdirectory(scene)
	add_subdirectory(physics)
	add_subdirectory(ide)
endif()

add_subdirectory(examples)

if(CMAKE_GENERATOR STREQUAL "Xcode")
	set(OT_RESOURCES resources/icon.icns)
endif()

add_executable(ot main.cpp ${OT_RESOURCES})
target_link_libraries(ot language network argparse)

if(OT_INCLUDE_UI)
	target_compile_definitions(ot PRIVATE OT_INCLUDE_UI)
	target_link_libraries(ot audio shaders gfx gui nodes scene physics ide)

	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
		if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15)
			target_link_options(ot PRIVATE LINKER:-no_warn_duplicate_libraries)
		endif()
	endif()
endif()

if(NOT MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
	add_custom_command(TARGET ot POST_BUILD COMMAND strip -x $<TARGET_FILE:ot>)
endif()

if(CMAKE_GENERATOR STREQUAL "Xcode")
	set_target_properties(ot PROPERTIES OUTPUT_NAME "ObjectTalk")
	set_target_properties(ot PROPERTIES MACOSX_BUNDLE ON)
	set_target_properties(ot PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/info.plist)
	set_target_properties(ot PROPERTIES XCODE_GENERATE_SCHEME ON)
	set_target_properties(ot PROPERTIES XCODE_ATTRIBUTE_EXECUTABLE_NAME "ot")
	set_target_properties(ot PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.goosje.ot")
	set_source_files_properties(${CMAKE_SOURCE_DIR}/resources/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

if(MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ot)
endif()

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_subdirectory(tests)

set_target_properties(examples tests PROPERTIES FOLDER other)
