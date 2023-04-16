#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(imguieditor GIT_REPOSITORY "https://github.com/goossens/ImGuiColorTextEdit")
FetchContent_MakeAvailable(imguieditor)

set(IMGUIEDITOR_SOURCES
	${imguieditor_SOURCE_DIR}/TextEditor.h
	${imguieditor_SOURCE_DIR}/TextEditor.cpp)

add_library(imguieditor STATIC ${IMGUIEDITOR_SOURCES})
add_dependencies(imguieditor imgui)
target_include_directories(imguieditor PUBLIC ${imguieditor_SOURCE_DIR})
target_link_libraries(imguieditor imgui)

set_target_properties(imguieditor PROPERTIES FOLDER deps)
