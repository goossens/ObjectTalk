#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

include(FetchContent)

include(deps/libuv/CMakeLists.txt)

if(OT_GUI)
	include(deps/glfw/CMakeLists.txt)
	include(deps/bgfx/CMakeLists.txt)
	include(deps/glm/CMakeLists.txt)
	include(deps/imgui/CMakeLists.txt)
	include(deps/imguifiledialog/CMakeLists.txt)
	include(deps/tinyobjloader/CMakeLists.txt)
	include(deps/box2d/CMakeLists.txt)
	include(deps/tweeny/CMakeLists.txt)
#	include(deps/assimp/CMakeLists.txt)
endif()
