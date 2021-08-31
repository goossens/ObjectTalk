#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(imgui GIT_REPOSITORY "https://github.com/ocornut/imgui" GIT_TAG "v1.84.2")
FetchContent_GetProperties(imgui)

if (NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
endif()

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
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h)

if (APPLE)
	list(APPEND IMGUI_SOURCES
		${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm
		${imgui_SOURCE_DIR}/backends/imgui_impl_metal.h)
endif()

if (UNIX AND NOT APPLE)
	# for Linux, BSD, Solaris, Minix
endif()

if (WIN32)
	# for Windows
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})
include_directories(${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)

if (APPLE)
	set_property (TARGET imgui APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endif()
