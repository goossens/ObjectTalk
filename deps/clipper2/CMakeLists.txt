#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(clipper2 QUIET GIT_REPOSITORY "https://github.com/AngusJohnson/Clipper2" GIT_TAG "Clipper2_1.5.2")

file(GLOB CLIPPER2_SOURCES
	${clipper2_SOURCE_DIR}/CPP/Clipper2Lib/src/*.cpp
	${clipper2_SOURCE_DIR}/CPP/Clipper2Lib/include/clipper2/*.h)

add_library(clipper2 STATIC ${CLIPPER2_SOURCES})
target_compile_definitions(clipper2 PUBLIC "CLIPPER2_PAR=-1")
target_include_directories(clipper2 PUBLIC ${clipper2_SOURCE_DIR}/CPP/Clipper2Lib/include)

set_target_properties(clipper2 PROPERTIES FOLDER deps)
