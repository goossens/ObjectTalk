#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB IMGUI_FILE_SELECTOR_SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(imguifileselector STATIC ${IMGUI_FILE_SELECTOR_SOURCES})
target_include_directories(imguifileselector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imguifileselector imgui)
set_target_properties(imguifileselector PROPERTIES FOLDER 3rdParty)

target_compile_options(imguifileselector PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Werror -Wall -Wextra>
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)
