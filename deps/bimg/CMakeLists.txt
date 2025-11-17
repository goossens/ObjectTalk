#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(bimg QUIET GIT_REPOSITORY "https://github.com/bkaradzic/bimg" GIT_TAG "5060d17")

file(GLOB BIMG_SOURCES
	${bimg_SOURCE_DIR}/src/*.cpp
	${bimg_SOURCE_DIR}/include/bimg/*.h
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.h
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include/*.h
	${bimg_SOURCE_DIR}/3rdparty/edtaa3/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/edtaa3/*.h
	${bimg_SOURCE_DIR}/3rdparty/etc1/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/etc1/*.h
	${bimg_SOURCE_DIR}/3rdparty/etc2/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/etc2/*.h
	${bimg_SOURCE_DIR}/3rdparty/iqa/source/*.c
	${bimg_SOURCE_DIR}/3rdparty/iqa/include/*.h
	${bimg_SOURCE_DIR}/3rdparty/libsquish/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/libsquish/*.h
	${bimg_SOURCE_DIR}/3rdparty/libsquish/*.inl
	${bimg_SOURCE_DIR}/3rdparty/nvtt/bc6h/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/nvtt/bc6h/*.h
	${bimg_SOURCE_DIR}/3rdparty/nvtt/bc7/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/nvtt/bc7/*.h
	${bimg_SOURCE_DIR}/3rdparty/nvtt/nvcore/*.h
	${bimg_SOURCE_DIR}/3rdparty/nvtt/nvcore/*.inl
	${bimg_SOURCE_DIR}/3rdparty/nvtt/nvmath/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/nvtt/nvmath/*.h
	${bimg_SOURCE_DIR}/3rdparty/nvtt/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/nvtt/*.h
	${bimg_SOURCE_DIR}/3rdparty/pvrtc/*.cpp
	${bimg_SOURCE_DIR}/3rdparty/pvrtc/*.h
	${bimg_SOURCE_DIR}/3rdparty/tinyexr/*.c
	${bimg_SOURCE_DIR}/3rdparty/tinyexr/*.h
	${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps/miniz/*.c
	${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps/miniz/*.h)

list(REMOVE_ITEM BIMG_SOURCES ${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/astcenc_diagnostic_trace.cpp)

add_library(bimg STATIC ${BIMG_SOURCES})

target_compile_options(bimg PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wall -Wno-unused -Wno-shorten-64-to-32>)

target_include_directories(bimg PUBLIC
	${bimg_SOURCE_DIR}/include
	${bimg_SOURCE_DIR}/3rdparty
	${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include
	${bimg_SOURCE_DIR}/3rdparty/iqa/include
	${bimg_SOURCE_DIR}/3rdparty/nvtt
	${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps/miniz)

target_link_libraries(bimg PUBLIC bx)

set_target_properties(bimg PROPERTIES FOLDER "deps")
