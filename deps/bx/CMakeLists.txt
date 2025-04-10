#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(bx QUIET GIT_REPOSITORY "https://github.com/bkaradzic/bx")

file(GLOB BX_SOURCES ${bx_SOURCE_DIR}/src/*.cpp ${bx_SOURCE_DIR}/include/bx/*.h  ${bx_SOURCE_DIR}/include/bx/inline/*.inl)
list(REMOVE_ITEM BX_SOURCES ${bx_SOURCE_DIR}/src/amalgamated.cpp)

add_library(bx STATIC ${BX_SOURCES})
target_compile_definitions(bx PUBLIC "BX_CONFIG_DEBUG=$<CONFIG:Debug>")
target_compile_options(bx PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wno-gnu-zero-variadic-macro-arguments>)
target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include ${bx_SOURCE_DIR}/3rdparty)

if(WIN32)
	target_link_libraries(bx PUBLIC psapi)
	target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include/compat/msvc)

	target_compile_options(bx PUBLIC "/wd4324")
	target_compile_options(bx PUBLIC "/Zc:__cplusplus")
	target_compile_options(bx PUBLIC "/Zc:preprocessor")

	target_compile_definitions(bx PUBLIC "__STDC_LIMIT_MACROS")
	target_compile_definitions(bx PUBLIC "__STDC_FORMAT_MACROS")
	target_compile_definitions(bx PUBLIC "__STDC_CONSTANT_MACROS")

elseif(UNIX AND NOT APPLE)
	target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include/compat/linux)

elseif(APPLE)
	target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include/compat/osx)
endif()

set_target_properties(bx PROPERTIES FOLDER deps)
