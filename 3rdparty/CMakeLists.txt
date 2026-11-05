#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_subdirectory(isocline)
add_subdirectory(llhttp)
add_subdirectory(multipart)

if(OT_INCLUDE_UI)
	add_subdirectory(imguieditor)
	add_subdirectory(imguiwidgets)
	add_subdirectory(nanovg)
	add_subdirectory(pffft)
	add_subdirectory(convolver)
	add_subdirectory(delaunator)
	add_subdirectory(tweeny)
endif()
