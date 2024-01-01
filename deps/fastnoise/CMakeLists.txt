#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(fastnoise GIT_REPOSITORY "https://github.com/Auburn/FastNoiseLite")
FetchContent_MakeAvailable(fastnoise)

add_library(fastnoise INTERFACE)
target_include_directories(fastnoise INTERFACE ${fastnoise_SOURCE_DIR}/Cpp)

file(GLOB FASTNOISE_SOURCES ${fastnoise_SOURCE_DIR}/Cpp/*.h)
add_custom_target(fastnoise_src SOURCES ${FASTNOISE_SOURCES})
source_group(TREE "${fastnoise_SOURCE_DIR}/Cpp" FILES ${FASTNOISE_SOURCES})
set_target_properties(fastnoise_src PROPERTIES FOLDER deps)
