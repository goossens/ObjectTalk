#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_executable(main main.cpp)
add_dependencies(main ot http)
target_link_libraries(main ot http argparse)

if(OT_GUI)
	add_dependencies(main shaders gfx graphics input math graph scene ide)
	target_compile_definitions(main PRIVATE INCLUDE_GUI)
	target_link_libraries(main shaders gfx graphics input math graph scene ide)
endif()

set_target_properties(main PROPERTIES OUTPUT_NAME ot)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_NAME ot)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set_target_properties(main PROPERTIES XCODE_GENERATE_SCHEME ON)
set_target_properties(main PROPERTIES XCODE_SCHEME_EXECUTABLE "${CMAKE_BINARY_DIR}/bin/ot")
set_target_properties(main PROPERTIES XCODE_SCHEME_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set_target_properties(main PROPERTIES VS_DEBUGGER_COMMAND "${CMAKE_BINARY_DIR}/bin/ot")

add_custom_command(TARGET main POST_BUILD COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}> ARGS ${CMAKE_BINARY_DIR}/bin/ot)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES main.cpp)
