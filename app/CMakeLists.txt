#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_executable(ot ot.cpp)
add_dependencies(ot libot http)

if(PRIVATE_UV)
	add_dependencies(ot uv)
endif()

if(OT_GUI)
	add_dependencies(ot gui glfw bgfx glm imgui tinyobjloader tweeny)
endif()

target_link_libraries(ot PRIVATE libot ${UV_LIBRARY})

if(APPLE)
	set_target_properties(ot PROPERTIES
		XCODE_GENERATE_SCHEME TRUE
	XCODE_SCHEME_ARGUMENTS "${CMAKE_SOURCE_DIR}/examples/3d/sky.ot")
endif()

set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

target_compile_features(ot PRIVATE cxx_std_17)
set_target_properties(ot PROPERTIES CXX_EXTENSIONS OFF)

install(TARGETS ot DESTINATION bin)
