#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB IMGUI_WIDGET_SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(imguiwidgets STATIC ${IMGUI_WIDGET_SOURCES})
target_include_directories(imguiwidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imguiwidgets imgui)
set_target_properties(imguiwidgets PROPERTIES FOLDER 3rdParty)
