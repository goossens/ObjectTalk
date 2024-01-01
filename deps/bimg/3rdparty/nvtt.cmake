#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(
	GLOB
	NVTT_SOURCES
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
)

add_library(nvtt STATIC ${NVTT_SOURCES})
target_include_directories(nvtt PUBLIC ${bimg_SOURCE_DIR}/3rdparty ${bimg_SOURCE_DIR}/3rdparty/nvtt)
target_link_libraries(nvtt PUBLIC bx)

set_target_properties(nvtt PROPERTIES FOLDER "deps/bimg-3rdparty")
