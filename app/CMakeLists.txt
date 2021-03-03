#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_executable(ot ot.cpp)
add_dependencies(ot http system)

set_target_properties(ot PROPERTIES
	XCODE_GENERATE_SCHEME TRUE
	XCODE_SCHEME_ARGUMENTS "${CMAKE_SOURCE_DIR}/examples/http.ot"
)

target_compile_features(ot PRIVATE cxx_std_17)
set_target_properties(ot PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(ot PRIVATE m whereami)

install(TARGETS ot DESTINATION bin)
