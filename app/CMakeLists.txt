#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_executable(ot ot.cpp)
add_dependencies(ot libot)

target_link_libraries(ot libot)

if(OT_GUI)
	add_dependencies(ot gui ide)
	target_compile_definitions(ot PRIVATE INCLUDE_GUI)
	target_link_libraries(ot gui ide)
endif()

set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set_target_properties(ot PROPERTIES XCODE_GENERATE_SCHEME TRUE)
set_target_properties(ot PROPERTIES XCODE_SCHEME_EXECUTABLE "${CMAKE_BINARY_DIR}/bin/ot")
set_target_properties(ot PROPERTIES XCODE_SCHEME_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set_target_properties(ot PROPERTIES VS_DEBUGGER_COMMAND "${CMAKE_BINARY_DIR}/bin/ot")

install(TARGETS ot DESTINATION bin)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ot.cpp)
