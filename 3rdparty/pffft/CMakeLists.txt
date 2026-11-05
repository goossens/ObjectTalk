#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB PFFFT_SOURCES CONFIGURE_DEPENDS *.c *.h)
add_library(pffft STATIC ${PFFFT_SOURCES})
target_include_directories(pffft PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(pffft PROPERTIES FOLDER 3rdParty)
