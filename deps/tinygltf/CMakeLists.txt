#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tinygltf GIT_REPOSITORY "https://github.com/syoyo/tinygltf" GIT_TAG "v2.8.0")
FetchContent_GetProperties(tinygltf)

if(NOT tinygltf_POPULATED)
	FetchContent_Populate(tinygltf)
endif()

add_library(tinygltf INTERFACE)
target_include_directories(tinygltf INTERFACE ${tinygltf_SOURCE_DIR})

file(GLOB TINYGLTF_SOURCES ${tinygltf_SOURCE_DIR}/*.h)

add_custom_target(tinygltf_src SOURCES ${TINYGLTF_SOURCES})
set_target_properties(tinygltf_src PROPERTIES FOLDER deps)
