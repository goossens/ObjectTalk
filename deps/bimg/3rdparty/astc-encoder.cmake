#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB ASTC_ENCODER_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.h
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include/*.h)

add_library(astc-encoder STATIC ${ASTC_ENCODER_SOURCES})
target_include_directories(astc-encoder PUBLIC ${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include)

set_target_properties(astc-encoder PROPERTIES FOLDER "deps/bimg-3rdparty")
