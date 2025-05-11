#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(manifold QUIET GIT_REPOSITORY "https://github.com/elalish/manifold" GIT_TAG "1d18e60")

file(GLOB MANIFOLD_SOURCES
	${manifold_SOURCE_DIR}/src/*.cpp
	${manifold_SOURCE_DIR}/src/*.h
	${manifold_SOURCE_DIR}/src/cross_section/cross_section.cpp
	${manifold_SOURCE_DIR}/include/manifold/*.h)

add_library(manifold STATIC ${MANIFOLD_SOURCES})
target_compile_definitions(manifold PUBLIC "MANIFOLD_PAR=-1")

target_compile_options(manifold PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall -Wno-unknown-warning-option -Wno-unused -Wno-shorten-64-to-32>
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/wd4267>)

target_include_directories(manifold PUBLIC ${manifold_SOURCE_DIR}/include)
target_link_libraries(manifold PUBLIC clipper2)

set_target_properties(manifold PROPERTIES FOLDER deps)
