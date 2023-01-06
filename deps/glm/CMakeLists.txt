#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_TAG "0.9.9.8")
FetchContent_GetProperties(glm)

if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
endif()

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})

file(GLOB GLM_SOURCES
	${glm_SOURCE_DIR}/glm/*.hpp
	${glm_SOURCE_DIR}/glm/*/*.h
	${glm_SOURCE_DIR}/glm/*/*.hpp
	${glm_SOURCE_DIR}/glm/*/*.inl)

add_custom_target(glm_src SOURCES ${GLM_SOURCES})
source_group(TREE "${glm_SOURCE_DIR}/glm" FILES ${GLM_SOURCES})
set_target_properties(glm_src PROPERTIES FOLDER deps)
