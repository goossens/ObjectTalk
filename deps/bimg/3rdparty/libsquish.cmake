#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB SQUISH_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/libsquish/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/libsquish/*.h
	${bimg_SOURCE_DIR}/3rdparty/libsquish/*.inl)

add_library(squish STATIC ${SQUISH_SOURCES})
target_include_directories(squish PUBLIC ${bimg_SOURCE_DIR}/3rdparty)

set_target_properties(squish PROPERTIES FOLDER "deps/bimg-3rdparty")
