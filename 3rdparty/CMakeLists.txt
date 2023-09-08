#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_library(llhttp STATIC llhttp/api.c llhttp/http.c llhttp/llhttp.c llhttp/llhttp.h)
target_include_directories(llhttp PUBLIC llhttp)
set_target_properties(llhttp PROPERTIES FOLDER 3rdParty)

add_library(multipart STATIC multipart/multipartparser.c multipart/multipartparser.h)
target_include_directories(multipart PUBLIC multipart)
set_target_properties(multipart PROPERTIES FOLDER 3rdParty)

if(OT_GUI)
	add_library(imguieditor STATIC imguieditor/TextEditor.cpp imguieditor/TextEditor.h imguieditor/LanguageDefinitions.cpp)
	add_dependencies(imguieditor imgui boost_regex)
	target_include_directories(imguieditor PUBLIC imguieditor)
	target_link_libraries(imguieditor imgui boost_regex)
	set_target_properties(imguieditor PROPERTIES FOLDER 3rdParty)
endif()
