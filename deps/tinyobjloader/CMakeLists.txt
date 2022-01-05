#	ObjectTalk GUI Module
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tinyobjloader GIT_REPOSITORY "https://github.com/tinyobjloader/tinyobjloader")
FetchContent_GetProperties(tinyobjloader)

if(NOT tinyobjloader_POPULATED)
	FetchContent_Populate(tinyobjloader)
	add_subdirectory(${tinyobjloader_SOURCE_DIR} ${tinyobjloader_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(${tinyobjloader_SOURCE_DIR})

set_target_properties(tinyobjloader PROPERTIES FOLDER deps)
set_target_properties(uninstall PROPERTIES FOLDER deps/tinyobjloader)
