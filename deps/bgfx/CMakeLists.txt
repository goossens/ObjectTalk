#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(bgfx GIT_REPOSITORY "https://github.com/bkaradzic/bgfx")
FetchContent_MakeAvailable(bgfx)

file(GLOB BGFX_SOURCES
	${bgfx_SOURCE_DIR}/src/*.cpp
	${bgfx_SOURCE_DIR}/src/*.h
	${bgfx_SOURCE_DIR}/include/bgfx/*.h
	${bgfx_SOURCE_DIR}/include/bgfx/c99/*.h
	${bgfx_SOURCE_DIR}/examples/common/debugdraw/debugdraw.cpp
	${bgfx_SOURCE_DIR}/examples/common/debugdraw/debugdraw.h)

list(REMOVE_ITEM BGFX_SOURCES ${bgfx_SOURCE_DIR}/src/amalgamated.cpp)

if (APPLE)
	set_source_files_properties(${bgfx_SOURCE_DIR}/src/renderer_vk.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
	file(GLOB BGFX_MAC_SOURCES ${bgfx_SOURCE_DIR}/src/*.mm)
	list(REMOVE_ITEM BGFX_MAC_SOURCES ${bgfx_SOURCE_DIR}/src/amalgamated.mm)
	list(APPEND BGFX_SOURCES ${BGFX_MAC_SOURCES})
endif()

if(NOT UNIX OR APPLE)
	list(REMOVE_ITEM BGFX_SOURCES ${bgfx_SOURCE_DIR}/src/glcontext_glx.cp)
endif()

add_library(bgfx STATIC ${BGFX_SOURCES})
add_dependencies(bgfx bx bimg)

target_include_directories(bgfx
	PRIVATE
		${bgfx_SOURCE_DIR}/3rdparty
		${bgfx_SOURCE_DIR}/3rdparty/khronos
	PUBLIC
		${bgfx_SOURCE_DIR}/include
		${bgfx_SOURCE_DIR}/examples/common/debugdraw)

target_link_libraries(bgfx PUBLIC bx bimg)

# suppress compiler warnings
target_compile_definitions(bgfx PRIVATE "BGFX_CONFIG_RENDERER_OPENGLES_MIN_VERSION=1")

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
	find_package(OpenGL REQUIRED)
	target_link_libraries(bgfx PUBLIC ${X11_LIBRARIES} ${OPENGL_LIBRARIES})

	target_include_directories(bgfx
		PRIVATE
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include/directx
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include/wsl/stubs)

endif()

if(WIN32)
	target_compile_definitions(bgfx PRIVATE "_CRT_SECURE_NO_WARNINGS")

	target_include_directories(bgfx
		PRIVATE
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include/directx)
endif()

set_target_properties(bgfx PROPERTIES FOLDER deps)
