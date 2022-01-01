#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_executable(ot ot.cpp)
add_dependencies(ot libot)

target_link_libraries(ot PUBLIC libot)

set_target_properties(ot PROPERTIES ENABLE_EXPORTS ON)
set_target_properties(ot PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

install(TARGETS ot DESTINATION bin)
