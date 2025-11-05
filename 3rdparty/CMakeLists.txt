#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_library(isocline STATIC isocline/isocline.c isocline/isocline.h)
target_include_directories(isocline PUBLIC isocline)
set_target_properties(isocline PROPERTIES FOLDER 3rdParty)

add_library(llhttp STATIC llhttp/api.c llhttp/http.c llhttp/llhttp.c llhttp/llhttp.h)
target_include_directories(llhttp PUBLIC llhttp)
set_target_properties(llhttp PROPERTIES FOLDER 3rdParty)

add_library(multipart STATIC multipart/multipartparser.c multipart/multipartparser.h)
target_include_directories(multipart PUBLIC multipart)
set_target_properties(multipart PROPERTIES FOLDER 3rdParty)

if(OT_INCLUDE_UI)
	set(IMGUI_EDITOR_HOME "imguieditor")
	file(GLOB IMGUI_EDITOR_SOURCES ${IMGUI_EDITOR_HOME}/*.cpp ${IMGUI_EDITOR_HOME}/*.h)
	add_library(imguieditor STATIC ${IMGUI_EDITOR_SOURCES})
	target_include_directories(imguieditor PUBLIC ${IMGUI_EDITOR_HOME})
	target_link_libraries(imguieditor imgui)
	set_target_properties(imguieditor PROPERTIES FOLDER 3rdParty)

	target_compile_options(imguieditor PRIVATE
		$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Werror -Wall -Wextra>
		$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wpedantic>
		$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

	file(GLOB IMGUI_WIDGET_SOURCES imguiwidgets/*.cpp imguiwidgets/*.h)
	add_library(imguiwidgets STATIC ${IMGUI_WIDGET_SOURCES})
	target_include_directories(imguiwidgets PUBLIC imguiwidgets)
	target_link_libraries(imguiwidgets imgui)
	set_target_properties(imguiwidgets PROPERTIES FOLDER 3rdParty)

	file(GLOB MD4C_SOURCES md4c/*.c md4c/*.h)
	add_library(md4c STATIC ${MD4C_SOURCES})
	target_include_directories(md4c PUBLIC md4c)
	set_target_properties(md4c PROPERTIES FOLDER 3rdParty)

	if(CMAKE_GENERATOR STREQUAL Xcode)
		set_target_properties(md4c PROPERTIES XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO")
	endif()

	file(GLOB DELAUNATOR_SOURCES delaunator/*.h delaunator/*.cpp)
	add_library(delaunator STATIC ${DELAUNATOR_SOURCES})
	target_include_directories(delaunator PUBLIC delaunator)
	set_target_properties(delaunator PROPERTIES FOLDER deps)

	add_library(tweeny INTERFACE)
	target_include_directories(tweeny INTERFACE tweeny)
	file(GLOB TWEENY_SOURCES tweeny/*.h tweeny/*.tcc)
	add_custom_target(tweeny_src SOURCES ${TWEENY_SOURCES})
	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/tweeny" FILES ${TWEENY_SOURCES})
	set_target_properties(tweeny_src PROPERTIES FOLDER deps)
endif()
