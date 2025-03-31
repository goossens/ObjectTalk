#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(entt GIT_REPOSITORY "https://github.com/skypjack/entt" GIT_TAG "v3.15.0")
FetchContent_MakeAvailable(entt)

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${entt_SOURCE_DIR}/src)

file(GLOB ENTT_SOURCES
	${entt_SOURCE_DIR}/src/entt/*.hpp
	${entt_SOURCE_DIR}/src/entt/*/*.h
	${entt_SOURCE_DIR}/src/entt/*/*.hpp)

add_custom_target(entt_src SOURCES ${ENTT_SOURCES})
source_group(TREE "${entt_SOURCE_DIR}/src/entt" FILES ${ENTT_SOURCES})
set_target_properties(entt_src PROPERTIES FOLDER deps)
