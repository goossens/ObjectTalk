#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

include(FetchContent)

add_subdirectory(argparse)
add_subdirectory(fmt)
add_subdirectory(libuv)
add_subdirectory(json)

if(OT_INCLUDE_UI)
	add_subdirectory(bsthreadpool)
	add_subdirectory(sdl)
	add_subdirectory(bx)
	add_subdirectory(bimg)
	add_subdirectory(bgfx)
	add_subdirectory(glm)
	add_subdirectory(tess2)
	add_subdirectory(plutovg)
	add_subdirectory(lunasvg)
	add_subdirectory(freetype)
	add_subdirectory(imgui)
	add_subdirectory(imguifiledialog)
	add_subdirectory(imguizmo)
	add_subdirectory(assimp)
	add_subdirectory(jolt)
	add_subdirectory(box2d)
	add_subdirectory(entt)
endif()
