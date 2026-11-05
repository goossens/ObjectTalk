#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB CONVOLVER_SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(convolver STATIC ${CONVOLVER_SOURCES})
target_include_directories(convolver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(convolver PROPERTIES FOLDER 3rdParty)
