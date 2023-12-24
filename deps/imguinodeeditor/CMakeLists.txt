#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

#FetchContent_Declare(imguinodeeditor GIT_REPOSITORY "https://github.com/thedmd/imgui-node-editor" GIT_TAG "v0.9.3")
FetchContent_Declare(imguinodeeditor GIT_REPOSITORY "https://github.com/thedmd/imgui-node-editor")
FetchContent_Populate(imguinodeeditor)

set(IMGUINODEEDITOR_SOURCES
	${imguinodeeditor_SOURCE_DIR}/crude_json.cpp
	${imguinodeeditor_SOURCE_DIR}/crude_json.h
	${imguinodeeditor_SOURCE_DIR}/imgui_bezier_math.h
	${imguinodeeditor_SOURCE_DIR}/imgui_bezier_math.inl
	${imguinodeeditor_SOURCE_DIR}/imgui_canvas.cpp
	${imguinodeeditor_SOURCE_DIR}/imgui_canvas.cpp
	${imguinodeeditor_SOURCE_DIR}/imgui_canvas.h
	${imguinodeeditor_SOURCE_DIR}/imgui_canvas.h
	${imguinodeeditor_SOURCE_DIR}/imgui_extra_math.h
	${imguinodeeditor_SOURCE_DIR}/imgui_extra_math.inl
	${imguinodeeditor_SOURCE_DIR}/imgui_node_editor_api.cpp
	${imguinodeeditor_SOURCE_DIR}/imgui_node_editor_internal.h
	${imguinodeeditor_SOURCE_DIR}/imgui_node_editor_internal.inl
	${imguinodeeditor_SOURCE_DIR}/imgui_node_editor.cpp
	${imguinodeeditor_SOURCE_DIR}/imgui_node_editor.h)

add_library(imguinodeeditor STATIC ${IMGUINODEEDITOR_SOURCES})
add_dependencies(imguinodeeditor imgui)

target_compile_definitions(imguinodeeditor PUBLIC CUSTOM_IMGUIFILEDIALOG_CONFIG="IgfdConfig.h")
target_include_directories(imguinodeeditor PUBLIC ${imguinodeeditor_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(imguinodeeditor imgui)

set_target_properties(imguinodeeditor PROPERTIES FOLDER deps)
