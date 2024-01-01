#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tweeny GIT_REPOSITORY "https://github.com/mobius3/tweeny")
FetchContent_Populate(tweeny)

add_library(tweeny INTERFACE)
target_include_directories(tweeny INTERFACE ${tweeny_SOURCE_DIR}/include)

file(GLOB TWEENY_SOURCES
	${tweeny_SOURCE_DIR}/include/*.h
	${tweeny_SOURCE_DIR}/include/*/*.tcc)

add_custom_target(tweeny_src SOURCES ${TWEENY_SOURCES})
source_group(TREE "${tweeny_SOURCE_DIR}/include" FILES ${TWEENY_SOURCES})
set_target_properties(tweeny_src PROPERTIES FOLDER deps)
