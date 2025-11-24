#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

include(FetchContent)

function(process package)
	message(STATUS "===========================")
	message(STATUS "Configuring ${package}")
	message(STATUS "===========================")
	add_subdirectory(${package})
endfunction()

process(argparse)
process(fmt)
process(json)
process(libuv)
process(httplib)

if(OT_INCLUDE_UI)
	process(bsthreadpool)
	process(glslang)
	process(spirvcross)
	process(sdl)
	process(shadercross)
	process(glm)
	process(tess2)
	process(plutovg)
	process(freetype)
	process(imgui)
	process(imguifiledialog)
	process(imguizmo)
	process(assimp)
	process(jolt)
	process(box2d)
	process(entt)
	process(clipper2)
	process(manifold)
endif()
