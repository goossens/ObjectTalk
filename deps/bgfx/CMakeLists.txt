#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(bgfx GIT_REPOSITORY "https://github.com/bkaradzic/bgfx")
FetchContent_Populate(bgfx)

file(GLOB BGFX_SOURCES
	${bgfx_SOURCE_DIR}/src/*.cpp
	${bgfx_SOURCE_DIR}/src/*.h
	${bgfx_SOURCE_DIR}/include/bgfx/*.h
	${bgfx_SOURCE_DIR}/include/bgfx/c99/*.h
	${bgfx_SOURCE_DIR}/examples/common/nanovg/*.cpp
	${bgfx_SOURCE_DIR}/examples/common/nanovg/*.h)

list(REMOVE_ITEM BGFX_SOURCES ${bgfx_SOURCE_DIR}/src/amalgamated.cpp)

if(APPLE)
	set_source_files_properties(${bgfx_SOURCE_DIR}/src/renderer_vk.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
	list(APPEND BGFX_SOURCES ${bgfx_SOURCE_DIR}/src/renderer_mtl.mm)
endif()

if(APPLE OR WIN32)
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
		${bgfx_SOURCE_DIR}/examples/common/nanovg)

target_link_libraries(bgfx PUBLIC bx bimg)

if(APPLE)
	target_compile_definitions(bgfx PRIVATE "BGFX_CONFIG_RENDERER_METAL=1")
endif()

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
	target_link_libraries(bgfx PUBLIC ${X11_LIBRARIES} ${OPENGL_LIBRARIES})
	target_compile_definitions(bgfx PRIVATE "BGFX_CONFIG_RENDERER_VULKAN=1")

	target_include_directories(bgfx
		PRIVATE
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include/directx
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include/wsl/stubs)

endif()

if(WIN32)
	target_compile_definitions(bgfx PRIVATE "_CRT_SECURE_NO_WARNINGS")
	target_compile_definitions(bgfx PRIVATE "BGFX_CONFIG_RENDERER_DIRECT3D11=1")

	target_include_directories(bgfx
		PRIVATE
		${bgfx_SOURCE_DIR}/3rdparty/directx-headers/include/directx)
endif()

set_target_properties(bgfx PROPERTIES FOLDER deps)
