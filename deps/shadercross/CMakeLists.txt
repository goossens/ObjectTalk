#	TextEditor - A syntax highlighting text editor for ImGui
#	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(shadercross QUIET GIT_REPOSITORY "https://github.com/libsdl-org/SDL_shadercross" GIT_TAG "main")

set(IMGUIFD_SOURCES
	${shadercross_SOURCE_DIR}/include/SDL3_shadercross/SDL_shadercross.h
	${shadercross_SOURCE_DIR}/src/SDL_shadercross.c)

add_library(shadercross STATIC ${IMGUIFD_SOURCES})
target_include_directories(shadercross PUBLIC ${shadercross_SOURCE_DIR}/include)
target_link_libraries(shadercross SDL3-static spirv-cross-c spirv-cross-core)

if(CMAKE_GENERATOR STREQUAL "Xcode")
	set_target_properties(shadercross PROPERTIES XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO")
endif()

set_target_properties(shadercross PROPERTIES FOLDER deps)
