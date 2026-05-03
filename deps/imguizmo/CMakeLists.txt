#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(imguizmo QUIET GIT_REPOSITORY "https://github.com/CedricGuillemet/ImGuizmo" GIT_TAG "1.9")

set(imguizmo_SOURCES
	${imguizmo_SOURCE_DIR}/src/ImGuizmo.h
	${imguizmo_SOURCE_DIR}/src/ImGuizmo.cpp)

add_library(imguizmo STATIC ${imguizmo_SOURCES})
target_include_directories(imguizmo PUBLIC ${imguizmo_SOURCE_DIR}/src)
target_link_libraries(imguizmo imgui)

set_target_properties(imguizmo PROPERTIES FOLDER deps)
