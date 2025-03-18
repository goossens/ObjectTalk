#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(MANIFOLD_CROSS_SECTION OFF CACHE BOOL "" FORCE)
set(MANIFOLD_TEST OFF CACHE BOOL "" FORCE)
set(MANIFOLD_DOWNLOADS OFF CACHE BOOL "" FORCE)

FetchContent_Populate(manifold QUIET GIT_REPOSITORY "https://github.com/elalish/manifold" GIT_TAG "v3.0.1")
add_subdirectory(${manifold_SOURCE_DIR} ${manifold_BINARY_DIR} EXCLUDE_FROM_ALL)

set_target_properties(manifold PROPERTIES FOLDER deps/manifold)
