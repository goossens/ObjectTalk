#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Populate(sdlmixer QUIET GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer" GIT_TAG "7d6e46a")
add_subdirectory(${sdlmixer_SOURCE_DIR} ${sdlmixer_BINARY_DIR} EXCLUDE_FROM_ALL)

set_target_properties(SDL3_mixer-static PROPERTIES FOLDER deps/sdlmixer)
