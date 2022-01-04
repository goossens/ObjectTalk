#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE OT_INCLUDES "*.h")
file(GLOB_RECURSE OT_SOURCES "*.cpp")

add_library(libot STATIC ${OT_INCLUDES} ${OT_SOURCES})

if (UNIX)
	set(OT_EXTRA_LIBS m dl)
endif()

target_link_libraries(libot PRIVATE uv_a llhttp multipart ${OT_EXTRA_LIBS})

set_target_properties(libot PROPERTIES PREFIX "")
set_target_properties(libot PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(libot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set_target_properties(libot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${OT_INCLUDES} ${OT_SOURCES})

# install(TARGETS libot DESTINATION lib)
# install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
