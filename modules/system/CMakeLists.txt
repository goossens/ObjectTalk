#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

find_path(UV_INCLUDES uv.h REQUIRED)
find_library(UV_LIBRARY uv REQUIRED)

file(GLOB_RECURSE SYSTEM_INCLUDES "*.h")
file(GLOB_RECURSE SYSTEM_SOURCES "*.cpp")

add_library(system MODULE ${SYSTEM_INCLUDES} ${SYSTEM_SOURCES})

target_compile_features(system PRIVATE cxx_std_17)
set_target_properties(system PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(system PRIVATE ${UV_INCLUDES})
target_link_libraries(system PRIVATE libot ${UV_LIBRARY})

set_target_properties(system PROPERTIES PREFIX "")

set_target_properties(system PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ot)
set_target_properties(system PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/ot)
set_target_properties(system PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/ot)

install(TARGETS system DESTINATION lib/ot)
