#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)

FetchContent_Populate(spirvcross QUIET GIT_REPOSITORY "https://github.com/KhronosGroup/SPIRV-Cross" GIT_TAG "main")
add_subdirectory(${spirvcross_SOURCE_DIR} ${spirvcross_BINARY_DIR} EXCLUDE_FROM_ALL)

set_target_properties(spirv-cross-c PROPERTIES FOLDER deps/spirvcross)
set_target_properties(spirv-cross-core PROPERTIES FOLDER deps/spirvcross)
set_target_properties(spirv-cross-glsl PROPERTIES FOLDER deps/spirvcross)
set_target_properties(spirv-cross-msl PROPERTIES FOLDER deps/spirvcross)
set_target_properties(spirv-cross-reflect PROPERTIES FOLDER deps/spirvcross)
set_target_properties(spirv-cross-util PROPERTIES FOLDER deps/spirvcross)
