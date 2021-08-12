#	ObjectTalk GUI Module
#	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tinyobjloader GIT_REPOSITORY "https://github.com/tinyobjloader/tinyobjloader")
FetchContent_MakeAvailable(tinyobjloader)
include_directories(${tinyobjloader_SOURCE_DIR})
