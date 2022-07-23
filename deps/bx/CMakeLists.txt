#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(bx GIT_REPOSITORY "https://github.com/bkaradzic/bx")
FetchContent_MakeAvailable(bx)

file(GLOB BX_SOURCES ${bx_SOURCE_DIR}/src/*.cpp ${bx_SOURCE_DIR}/include/bx/*.h)
list(REMOVE_ITEM BX_SOURCES ${bx_SOURCE_DIR}/src/amalgamated.cpp)

add_library(bx STATIC ${BX_SOURCES})
target_compile_definitions(bx PUBLIC "BX_CONFIG_DEBUG=$<CONFIG:Debug>")
target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include ${bx_SOURCE_DIR}/3rdparty)

if(WIN32)
	target_link_libraries(bx PUBLIC psapi)
	target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include/compat/msvc)

	target_compile_options(bx PUBLIC "/Zc:__cplusplus")

	target_compile_definitions(bx PUBLIC "__STDC_LIMIT_MACROS")
	target_compile_definitions(bx PUBLIC "__STDC_FORMAT_MACROS")
	target_compile_definitions(bx PUBLIC "__STDC_CONSTANT_MACROS")

elseif(APPLE)
	target_include_directories(bx PUBLIC ${bx_SOURCE_DIR}/include/compat/osx)
endif()

set_target_properties(bx PROPERTIES FOLDER deps)
