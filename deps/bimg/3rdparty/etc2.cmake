#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB ETC2_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/etc2/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/etc2/*.h)

add_library(etc2 STATIC ${ETC2_SOURCES})
target_include_directories(etc2 PUBLIC ${bimg_SOURCE_DIR}/3rdparty)
target_link_libraries(etc2 PUBLIC bx)

set_target_properties(etc2 PROPERTIES FOLDER "deps/bimg")
