#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB DELAUNATOR_SOURCES CONFIGURE_DEPENDS *.h *.cpp)
add_library(delaunator STATIC ${DELAUNATOR_SOURCES})
target_include_directories(delaunator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(delaunator PROPERTIES FOLDER 3rdParty)
