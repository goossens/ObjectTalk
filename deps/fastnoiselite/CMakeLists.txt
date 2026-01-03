#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(fastnoiselite QUIET GIT_REPOSITORY "https://github.com/Auburn/FastNoiseLite")

add_library(fastnoiselite INTERFACE)
target_include_directories(fastnoiselite INTERFACE ${fastnoiselite_SOURCE_DIR}/Cpp)

file(GLOB FASTNOISELITE_SOURCES ${fastnoiselite_SOURCE_DIR}/Cpp/*.h)
add_custom_target(fastnoiselite_src SOURCES ${FASTNOISELITE_SOURCES})
source_group(TREE "${fastnoiselite_SOURCE_DIR}/Cpp" FILES ${FASTNOISELITE_SOURCES})
set_target_properties(fastnoiselite_src PROPERTIES FOLDER deps)
