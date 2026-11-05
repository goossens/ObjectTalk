#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_library(isocline STATIC isocline.c isocline.h)
target_include_directories(isocline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(isocline PROPERTIES FOLDER 3rdParty)
