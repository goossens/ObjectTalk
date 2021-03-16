#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

find_path(UV_INCLUDES uv.h REQUIRED)
find_library(UV_LIBRARY uv REQUIRED)

file(GLOB_RECURSE HTTP_INCLUDES "*.h")
file(GLOB_RECURSE HTTP_SOURCES "*.cpp")

add_library(http MODULE ${HTTP_INCLUDES} ${HTTP_SOURCES})

target_include_directories(http PRIVATE ${UV_INCLUDES})
target_link_libraries(http PRIVATE libot ${UV_LIBRARY} llhttp multipart)

target_compile_features(http PRIVATE cxx_std_17)
set_target_properties(http PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(http PROPERTIES PREFIX "")

set_target_properties(http PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ot)
set_target_properties(http PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/ot)
set_target_properties(http PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/ot)

install(TARGETS http DESTINATION lib/ot)
