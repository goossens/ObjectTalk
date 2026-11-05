#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.


add_library(tweeny INTERFACE)
target_include_directories(tweeny INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB TWEENY_SOURCES CONFIGURE_DEPENDS *.h *.tcc)
add_custom_target(tweeny_src SOURCES ${TWEENY_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TWEENY_SOURCES})
set_target_properties(tweeny_src PROPERTIES FOLDER 3rdParty)
