#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(GLM_ENABLE_CXX_17 ON CACHE BOOL "" FORCE)

# FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_TAG "0.9.9.8")
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm")
FetchContent_MakeAvailable(glm)

target_compile_definitions(glm PUBLIC "GLM_ENABLE_EXPERIMENTAL")
set_target_properties(glm PROPERTIES FOLDER deps)
