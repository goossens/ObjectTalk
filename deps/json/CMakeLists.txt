#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(json GIT_REPOSITORY "https://github.com/nlohmann/json" GIT_TAG "v3.11.2")
FetchContent_GetProperties(json)

if(NOT json_POPULATED)
	FetchContent_Populate(json)
endif()

add_library(json INTERFACE)
target_include_directories(json INTERFACE ${json_SOURCE_DIR}/single_include)

file(GLOB JSON_SOURCES ${json_SOURCE_DIR}/single_include/nlohmann/*.hpp)

add_custom_target(json_src SOURCES ${JSON_SOURCES})
source_group(TREE "${json_SOURCE_DIR}/single_include/nlohmann" FILES ${JSON_SOURCES})
set_target_properties(json_src PROPERTIES FOLDER deps)
