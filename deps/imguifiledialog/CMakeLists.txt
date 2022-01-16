#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(imguifd GIT_REPOSITORY "https://github.com/goossens/ImGuiFileDialog" GIT_TAG "Lib_Only")
FetchContent_MakeAvailable(imguifd)

set(IMGUIFD_SOURCES
	${imguifd_SOURCE_DIR}/ImGuiFileDialog.h
	${imguifd_SOURCE_DIR}/ImGuiFileDialogConfig.h
	${imguifd_SOURCE_DIR}/ImGuiFileDialog.cpp)

add_library(imguifd STATIC ${IMGUIFD_SOURCES})
add_dependencies(imguifd imgui)
target_compile_definitions(imguifd PUBLIC CUSTOM_IMGUIFILEDIALOG_CONFIG="IgfdConfig.h")
include_directories(${imguifd_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})

set_target_properties(imguifd PROPERTIES FOLDER deps/imgui)
