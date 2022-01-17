#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(imgui GIT_REPOSITORY "https://github.com/ocornut/imgui" GIT_TAG "v1.86")
FetchContent_MakeAvailable(imgui)

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
	${imgui_SOURCE_DIR}/imstb_truetype.h)

add_library(imgui STATIC ${IMGUI_SOURCES})
include_directories(${imgui_SOURCE_DIR})

if (APPLE)
	set_property(TARGET imgui APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endif()

set_target_properties(imgui PROPERTIES FOLDER deps)
