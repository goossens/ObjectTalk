#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB LANGUAGE_INCLUDES */*.h)
file(GLOB LANGUAGE_SOURCES */*.cpp)

set(LANGUAGE_INCLUDE_DIRS "")

foreach(HEADER ${LANGUAGE_INCLUDES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND LANGUAGE_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES LANGUAGE_INCLUDE_DIRS)

add_library(language OBJECT ${LANGUAGE_INCLUDES} ${LANGUAGE_SOURCES})

if (UNIX)
	set(LANGUAGE_EXTRA_LIBS m)
endif()

target_include_directories(language PUBLIC ${LANGUAGE_INCLUDE_DIRS})
target_link_libraries(language PUBLIC uv_a json fmt isocline multipart llhttp ${LANGUAGE_EXTRA_LIBS})

set_target_properties(language PROPERTIES PREFIX "")
set_target_properties(language PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(language PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set_target_properties(language PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LANGUAGE_INCLUDES} ${LANGUAGE_SOURCES})
