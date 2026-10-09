#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(MESHOPT_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(meshoptimizer QUIET GIT_REPOSITORY "https://github.com/zeux/meshoptimizer" GIT_TAG "v1.3")
FetchContent_MakeAvailable(meshoptimizer)

target_compile_options(meshoptimizer PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wno-shorten-64-to-32>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/wd4244 /wd4267>)

set_target_properties(meshoptimizer PROPERTIES FOLDER deps)
