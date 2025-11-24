#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(imgui QUIET GIT_REPOSITORY "https://github.com/ocornut/imgui" GIT_TAG "v1.92.5")

set(IMGUI_SOURCES
	${imgui_SOURCE_DIR}/imgui.h
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_internal.h
	${imgui_SOURCE_DIR}/imstb_rectpack.h
	${imgui_SOURCE_DIR}/imstb_textedit.h
	${imgui_SOURCE_DIR}/imstb_truetype.h
	${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
	${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h)

add_library(imgui STATIC ${IMGUI_SOURCES})
# target_compile_definitions(imgui PUBLIC "IMGUI_USE_WCHAR32")
target_compile_definitions(imgui PUBLIC "IMGUI_DEFINE_MATH_OPERATORS")
target_compile_definitions(imgui PUBLIC "IMGUI_ENABLE_FREETYPE")
target_compile_definitions(imgui PUBLIC "IMGUI_DISABLE_OBSOLETE_FUNCTIONS")
target_compile_definitions(imgui PUBLIC "IMGUI_DISABLE_OBSOLETE_KEYIO")
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

target_link_libraries(imgui PRIVATE SDL3_Headers PUBLIC freetype)

set_target_properties(imgui PROPERTIES FOLDER deps)
