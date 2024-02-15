#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB ETC1_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/etc1/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/etc1/*.h)

add_library(etc1 STATIC ${ETC1_SOURCES})
target_include_directories(etc1 PUBLIC ${bimg_SOURCE_DIR}/3rdparty)

set_target_properties(etc1 PROPERTIES FOLDER "deps/bimg-3rdparty")
