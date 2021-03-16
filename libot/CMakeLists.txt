#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

find_path(UV_INCLUDES uv.h REQUIRED)
find_library(UV_LIBRARY uv REQUIRED)

file(GLOB_RECURSE OT_INCLUDES "*.h")
file(GLOB_RECURSE OT_SOURCES "*.cpp")

add_library(libot SHARED ${OT_INCLUDES} ${OT_SOURCES})

target_include_directories(libot PRIVATE ${UV_INCLUDES})
target_link_libraries(libot PRIVATE m dl ${UV_LIBRARY})

target_compile_features(libot PRIVATE cxx_std_17)

set_target_properties(libot PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(libot PROPERTIES PREFIX "")

set_target_properties(libot PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(libot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set_target_properties(libot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

install(TARGETS libot DESTINATION lib)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
