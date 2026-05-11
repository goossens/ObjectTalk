#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(tiff-tools OFF CACHE BOOL "" FORCE)
set(tiff-tests OFF CACHE BOOL "" FORCE)
set(tiff-contrib OFF CACHE BOOL "" FORCE)
set(tiff-docs OFF CACHE BOOL "" FORCE)

FetchContent_Populate(libtiff QUIET GIT_REPOSITORY "https://gitlab.com/libtiff/libtiff" GIT_TAG "v4.7.1")
add_subdirectory(${libtiff_SOURCE_DIR} ${libtiff_BINARY_DIR} EXCLUDE_FROM_ALL)

set_target_properties(tiff PROPERTIES FOLDER deps)
