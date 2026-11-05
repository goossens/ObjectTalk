#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_library(llhttp STATIC api.c http.c llhttp.c llhttp.h)
target_include_directories(llhttp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(llhttp PROPERTIES FOLDER 3rdParty)
