#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB PVRTC_SOURCES ${bimg_SOURCE_DIR}/3rdparty/pvrtc/*.cpp ${bimg_SOURCE_DIR}/3rdparty/pvrtc/*.h)

add_library( pvrtc STATIC ${PVRTC_SOURCES})
target_include_directories(pvrtc PUBLIC ${bimg_SOURCE_DIR}/3rdparty)

set_target_properties(pvrtc PROPERTIES FOLDER "deps/bimg-3rdparty")
