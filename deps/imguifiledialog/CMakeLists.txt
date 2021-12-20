#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(imguifd GIT_REPOSITORY "https://github.com/aiekick/ImGuiFileDialog" GIT_TAG "v0.5.6")
FetchContent_MakeAvailable(imguifd)

set(IMGUIFD_SOURCES
	${imguifd_SOURCE_DIR}/ImGuiFileDialog.h
	${imguifd_SOURCE_DIR}/ImGuiFileDialogConfig.h
	${imguifd_SOURCE_DIR}/ImGuiFileDialog.cpp)

add_library(imguifd STATIC ${IMGUIFD_SOURCES})
add_dependencies(imguifd glfw)
target_compile_definitions(imguifd PUBLIC CUSTOM_IMGUIFILEDIALOG_CONFIG="IgfdConfig.h")
include_directories(${imguifd_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})

if (APPLE)
	set_property(TARGET imguifd APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endif()
