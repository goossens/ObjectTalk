#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB NETWORK_INCLUDES CONFIGURE_DEPENDS */*.h)
file(GLOB NETWORK_SOURCES CONFIGURE_DEPENDS */*.cpp)

set(NETWORK_INCLUDE_DIRS "")

foreach(HEADER ${NETWORK_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND NETWORK_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES NETWORK_INCLUDE_DIRS)
set(NETWORK_INCLUDE_DIRS ${NETWORK_INCLUDE_DIRS} PARENT_SCOPE)

add_library(network OBJECT ${NETWORK_INCLUDES} ${NETWORK_SOURCES})

target_compile_options(network PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Werror -Wall -Wextra -Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Werror -Wall -Wextra -Wno-overloaded-virtual>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

target_include_directories(network PUBLIC ${NETWORK_INCLUDE_DIRS})
target_link_libraries(network PUBLIC language httplib llhttp multipart)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${NETWORK_INCLUDES} ${NETWORK_SOURCES})
