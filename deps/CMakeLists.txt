#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

include(FetchContent)

add_subdirectory(argparse)
add_subdirectory(libuv)
add_subdirectory(json)

if(OT_GUI)
	add_subdirectory(glfw)
	add_subdirectory(bx)
	add_subdirectory(bimg)
	add_subdirectory(bgfx)
	add_subdirectory(glm)
	add_subdirectory(tess2)
	add_subdirectory(imgui)
	add_subdirectory(imguifiledialog)
	add_subdirectory(imguizmo)
	add_subdirectory(assimp)
	add_subdirectory(box2d)
	add_subdirectory(tweeny)
	add_subdirectory(fastnoise)
	add_subdirectory(entt)
endif()
