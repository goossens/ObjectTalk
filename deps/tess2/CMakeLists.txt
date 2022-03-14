#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tess2 GIT_REPOSITORY "https://github.com/memononen/libtess2")
FetchContent_MakeAvailable(tess2)

file(GLOB TESS2_SOURCES
	${tess2_SOURCE_DIR}/Source/*.c
	${tess2_SOURCE_DIR}/Source/*.h
	${tess2_SOURCE_DIR}/Include/*.h)

add_library(tess2 STATIC ${TESS2_SOURCES})
target_include_directories(tess2 PUBLIC ${tess2_SOURCE_DIR}/Include)
set_target_properties(tess2 PROPERTIES FOLDER deps)
