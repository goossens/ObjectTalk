#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

include_directories(core geometries platform shaders simulation widgets world)

file(GLOB_RECURSE GUI_INCLUDES */*.h)
file(GLOB_RECURSE GUI_SOURCES module.cpp */*.cpp)

if (APPLE)
	# for MacOS
	list(APPEND GUI_SOURCES platform/macos.mm)

	find_library(COCOA Cocoa REQUIRED)
	find_library(QUARTZCORE QuartzCore REQUIRED)
	find_library(METAL Metal REQUIRED)
	find_library(METALKIT MetalKit REQUIRED)

	set(GUI_LIBS ${COCOA} ${QUARTZCORE} ${METAL} ${METALKIT})
endif()

if (UNIX AND NOT APPLE)
	# for Linux, BSD, Solaris, Minix
endif()

if (WIN32)
	# for Windows
endif()

add_library(gui MODULE ${GUI_INCLUDES} ${GUI_SOURCES})

target_link_libraries(gui libot ${UV_LIBRARY} glfw bgfx imgui tinyobjloader tweeny ${GUI_LIBS})

target_compile_features(gui PRIVATE cxx_std_17)
set_target_properties(gui PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(gui PROPERTIES PREFIX "")

set_target_properties(gui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ot)
set_target_properties(gui PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/ot)
set_target_properties(gui PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/ot)

install(TARGETS gui DESTINATION lib/ot)
