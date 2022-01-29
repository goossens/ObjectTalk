#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB
	ASTC_CODEC_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/astc-codec/src/decoder/*.cc
	${bimg_SOURCE_DIR}/3rdparty/astc-codec/src/decoder/*.h)

add_library(astc-codec STATIC ${ASTC_CODEC_SOURCES})

target_include_directories(astc-codec
	PUBLIC ${bimg_SOURCE_DIR}/3rdparty/astc-codec/include
	PUBLIC ${bimg_SOURCE_DIR}/3rdparty/astc-codec)

set_target_properties(astc-codec PROPERTIES FOLDER "deps/bimg-3rdparty")
