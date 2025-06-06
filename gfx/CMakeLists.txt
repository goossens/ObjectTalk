#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB GFX_INCLUDES */*.h)
file(GLOB GFX_SOURCES */*.cpp)

if(APPLE)
	list(APPEND GFX_SOURCES framework/OtMacos.mm)

	find_library(COCOA Cocoa REQUIRED)
	find_library(QUARTZCORE QuartzCore REQUIRED)
	find_library(METAL Metal REQUIRED)
	find_library(METALKIT MetalKit REQUIRED)

	set(GFX_LIBS ${COCOA} ${QUARTZCORE} ${METAL} ${METALKIT})
endif()

set(GFX_INCLUDE_DIRS "")

foreach(HEADER ${GFX_INCLUDES})
	get_filename_component(DIR ${HEADER} PATH)
	list(APPEND GFX_INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES GFX_INCLUDE_DIRS)
set(GFX_INCLUDE_DIRS ${GFX_INCLUDE_DIRS} PARENT_SCOPE)

add_library(gfx OBJECT ${GFX_INCLUDES} ${GFX_SOURCES})

target_compile_options(gfx PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Werror -Wall -Wextra>
	$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wpedantic>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>)

target_include_directories(gfx PUBLIC ${GFX_INCLUDE_DIRS})

target_link_libraries(gfx PUBLIC
	language
	network
	shaders
	SDL3-static
	bgfx
	imgui
	imguifd
	imguiwidgets
	glm
	delaunator
	tess2
	manifold
	lunasvg
	entt
	assimp
	md4c
	bsthreadpool
	tweeny
	${GFX_LIBS})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GFX_INCLUDES} ${GFX_SOURCES})
