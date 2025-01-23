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
#	set(IMGUI_EDITOR_HOME "imguieditor/dist")
	file(GLOB IMGUI_EDITOR_SOURCES ${IMGUI_EDITOR_HOME}/*.cpp ${IMGUI_EDITOR_HOME}/*.h)
	add_library(imguieditor STATIC ${IMGUI_EDITOR_SOURCES})
	target_include_directories(imguieditor PUBLIC ${IMGUI_EDITOR_HOME})
	target_link_libraries(imguieditor imgui)
	set_target_properties(imguieditor PROPERTIES FOLDER 3rdParty)

	file(GLOB IMGUI_WIDGET_SOURCES imguiwidgets/*.cpp imguiwidgets/*.h)
	add_library(imguiwidgets STATIC ${IMGUI_WIDGET_SOURCES})
	target_include_directories(imguiwidgets PUBLIC imguiwidgets)
	target_link_libraries(imguiwidgets imgui)
	set_target_properties(imguiwidgets PROPERTIES FOLDER 3rdParty)
endif()
