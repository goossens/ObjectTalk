#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(MESHOPT_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(meshoptimizer QUIET GIT_REPOSITORY "https://github.com/zeux/meshoptimizer" GIT_TAG "v1.3")
FetchContent_MakeAvailable(meshoptimizer)

set_target_properties(meshoptimizer PROPERTIES FOLDER deps)
