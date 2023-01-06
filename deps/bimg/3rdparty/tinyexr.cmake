#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE TINYEXR_SOURCES ${bimg_SOURCE_DIR}/3rdparty/tinyexr/*.c ${bimg_SOURCE_DIR}/3rdparty/tinyexr/*.h)

add_library(tinyexr STATIC ${TINYEXR_SOURCES})
target_include_directories(tinyexr PUBLIC ${bimg_SOURCE_DIR}/3rdparty ${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps/miniz)

set_target_properties(tinyexr PROPERTIES FOLDER "deps/bimg-3rdparty")
