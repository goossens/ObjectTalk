#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_library(multipart STATIC multipartparser.c multipartparser.h)
target_include_directories(multipart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(multipart PROPERTIES FOLDER 3rdParty)
