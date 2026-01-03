#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	process(fastnoiselite)
	process(glslang)
	process(spirvcross)
	process(sdl)
	process(sdlmixer)
	process(shadercross)
	process(glm)
	process(tess2)
	process(plutovg)
	process(freetype)
	process(imgui)
	process(imguifiledialog)
	process(imguiknobs)
	process(imguizmo)
	process(implot)
	process(assimp)
	process(jolt)
	process(box2d)
	process(entt)
	process(clipper2)
	process(manifold)
endif()
