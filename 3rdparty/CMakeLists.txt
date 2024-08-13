#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

if(OT_GUI)
	add_library(imguieditor STATIC imguieditor/TextEditor.cpp imguieditor/TextEditor.h imguieditor/LanguageDefinitions.cpp)
	target_compile_definitions(imguieditor PUBLIC IMGUI_EDITOR_NO_BOOST)
	target_include_directories(imguieditor PUBLIC imguieditor)
	target_link_libraries(imguieditor imgui)
	set_target_properties(imguieditor PROPERTIES FOLDER 3rdParty)

	file(GLOB IMGUI_WIDGET_SOURCES imguiwidgets/*.cpp imguiwidgets/*.h)
	add_library(imguiwidgets STATIC ${IMGUI_WIDGET_SOURCES})
	target_include_directories(imguiwidgets PUBLIC imguiwidgets)
	target_link_libraries(imguiwidgets imgui)
	set_target_properties(imguiwidgets PROPERTIES FOLDER 3rdParty)

	file(GLOB NANOVG_SOURCES nanovg/*.cpp nanovg/*.h)
	add_library(nanovg STATIC ${NANOVG_SOURCES})
	target_include_directories(nanovg PUBLIC nanovg)
	set_target_properties(nanovg PROPERTIES FOLDER 3rdParty)
endif()
