#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB ASTC_SOURCES ${bimg_SOURCE_DIR}/3rdparty/astc/*.cpp ${bimg_SOURCE_DIR}/3rdparty/astc/*.h)

add_library(astc STATIC ${ASTC_SOURCES})

set_target_properties(astc PROPERTIES FOLDER "deps/bimg-3rdparty")
