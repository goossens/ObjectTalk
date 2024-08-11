#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(bimg GIT_REPOSITORY "https://github.com/bkaradzic/bimg")
FetchContent_MakeAvailable(bimg)

include(3rdparty/astc-encoder.cmake)
include(3rdparty/edtaa3.cmake)
include(3rdparty/etc1.cmake)
include(3rdparty/etc2.cmake)
include(3rdparty/iqa.cmake)
include(3rdparty/libsquish.cmake)
include(3rdparty/nvtt.cmake)
include(3rdparty/pvrtc.cmake)
include(3rdparty/tinyexr.cmake)

file(GLOB BIMG_SOURCES
	${bimg_SOURCE_DIR}/src/*.cpp
	${bimg_SOURCE_DIR}/include/bimg/*.h)

add_library(bimg STATIC ${BIMG_SOURCES})
target_include_directories(bimg PUBLIC ${bimg_SOURCE_DIR}/include ${bimg_SOURCE_DIR}/3rdparty)
target_link_libraries(bimg PUBLIC bx PRIVATE astc-encoder edtaa3 etc1 etc2 iqa squish nvtt pvrtc tinyexr)

set_target_properties(bimg PROPERTIES FOLDER "deps")
