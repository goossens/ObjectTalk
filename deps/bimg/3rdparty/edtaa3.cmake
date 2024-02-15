#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB EDTAA3_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/edtaa3/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/edtaa3/*.h)

add_library(edtaa3 STATIC ${EDTAA3_SOURCES})
target_include_directories(edtaa3 PUBLIC ${bimg_SOURCE_DIR}/3rdparty>)

set_target_properties(edtaa3 PROPERTIES FOLDER "deps/bimg-3rdparty")
