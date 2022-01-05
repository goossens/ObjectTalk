#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(imguieditor GIT_REPOSITORY "https://github.com/BalazsJako/ImGuiColorTextEdit")
FetchContent_MakeAvailable(imguieditor)

set(IMGUIEDITOR_SOURCES
	${imguieditor_SOURCE_DIR}/TextEditor.h
	${imguieditor_SOURCE_DIR}/TextEditor.cpp)

add_library(imguieditor STATIC ${IMGUIEDITOR_SOURCES})
add_dependencies(imguieditor imgui)
include_directories(${imguieditor_SOURCE_DIR})

set_target_properties(imguieditor PROPERTIES FOLDER deps/imgui)
