#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(imguiknobs QUIET GIT_REPOSITORY "https://github.com/altschuler/imgui-knobs" GIT_TAG "main")

set(imguiknobs_SOURCES
	${imguiknobs_SOURCE_DIR}/imgui-knobs.h
	${imguiknobs_SOURCE_DIR}/imgui-knobs.cpp)

add_library(imguiknobs STATIC ${imguiknobs_SOURCES})
target_include_directories(imguiknobs PUBLIC ${imguiknobs_SOURCE_DIR})
target_link_libraries(imguiknobs imgui)

set_target_properties(imguiknobs PROPERTIES FOLDER deps)
