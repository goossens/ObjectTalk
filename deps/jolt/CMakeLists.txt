#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(CPP_EXCEPTIONS_ENABLED ON CACHE BOOL "" FORCE)

FetchContent_Declare(jolt GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics" GIT_TAG "v5.1.0")
FetchContent_MakeAvailable(jolt)

set(PHYSICS_REPO_ROOT ${jolt_SOURCE_DIR})
include(${PHYSICS_REPO_ROOT}/Jolt/Jolt.cmake)
set_target_properties(Jolt PROPERTIES FOLDER deps)
