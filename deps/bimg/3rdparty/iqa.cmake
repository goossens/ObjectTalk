#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file( GLOB IQA_SOURCES
	${bimg_SOURCE_DIR}/3rdparty/iqa/source/*.c
	${bimg_SOURCE_DIR}/3rdparty/iqa/include/*.h)

add_library(iqa STATIC ${IQA_SOURCES})
target_include_directories(iqa PUBLIC ${bimg_SOURCE_DIR}/3rdparty/iqa/include)

set_target_properties(iqa PROPERTIES FOLDER "deps/bimg")
