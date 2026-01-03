#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB AUDIO_INCLUDES CONFIGURE_DEPENDS */*.h)
file(GLOB AUDIO_SOURCES CONFIGURE_DEPENDS */*.cpp)

set(AUDIO_INCLUDE_DIRS "")

foreach(HEADER ${AUDIO_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND AUDIO_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES AUDIO_INCLUDE_DIRS)
set(AUDIO_INCLUDE_DIRS ${AUDIO_INCLUDE_DIRS} PARENT_SCOPE)

add_library(audio OBJECT ${AUDIO_INCLUDES} ${AUDIO_SOURCES})

target_compile_options(audio PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Werror -Wall -Wextra -Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Werror -Wall -Wextra -Wno-overloaded-virtual -Wno-error=array-bounds>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

target_include_directories(audio PUBLIC ${AUDIO_INCLUDE_DIRS})
target_link_libraries(audio PUBLIC language gfx SDL3::SDL3 SDL3_mixer-static pffft)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${AUDIO_INCLUDES} ${AUDIO_SOURCES})
