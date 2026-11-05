#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB NANOVG_SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(nanovg STATIC ${NANOVG_SOURCES})
target_compile_options(nanovg PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wno-gnu-anonymous-struct -Wno-nested-anon-types>)
target_compile_definitions(nanovg PRIVATE STBTT_STATIC)
target_include_directories(nanovg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nanovg SDL3::SDL3)
set_target_properties(nanovg PROPERTIES FOLDER 3rdParty)
