#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

target_compile_options(language PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Werror -Wall -Wextra>
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

if (UNIX)
	set(LANGUAGE_EXTRA_LIBS m)
endif()

target_include_directories(language PUBLIC ${LANGUAGE_INCLUDE_DIRS})
target_link_libraries(language PUBLIC uv_a httplib json fmt isocline multipart llhttp ${LANGUAGE_EXTRA_LIBS})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LANGUAGE_INCLUDES} ${LANGUAGE_SOURCES})
