#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(implot QUIET GIT_REPOSITORY "https://github.com/epezent/implot" GIT_TAG "v0.17")

set(implot_SOURCES
	${implot_SOURCE_DIR}/implot.h
	${implot_SOURCE_DIR}/implot_internal.h
	${implot_SOURCE_DIR}/implot.cpp
	${implot_SOURCE_DIR}/implot_items.cpp)

add_library(implot STATIC ${implot_SOURCES})
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot imgui)

set_target_properties(implot PROPERTIES FOLDER deps)
