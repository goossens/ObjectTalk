#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB IMGUI_EDITOR_SOURCES CONFIGURE_DEPENDS *.cpp *.h languages/*.h languages/*.re)
add_library(imguieditor STATIC ${IMGUI_EDITOR_SOURCES})
target_include_directories(imguieditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imguieditor imgui lsp)
set_target_properties(imguieditor PROPERTIES FOLDER 3rdParty)

target_compile_options(imguieditor PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Werror -Wall -Wextra>
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)
