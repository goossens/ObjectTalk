#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_WAVPACK OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDLMIXER_GME OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MOD_XMP OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC_LIBFLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC_DRFLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_STB OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_VORBISFILE OFF CACHE BOOL "" FORCE)

FetchContent_Populate(sdlmixer QUIET GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer" GIT_TAG "19a1a38")
add_subdirectory(${sdlmixer_SOURCE_DIR} ${sdlmixer_BINARY_DIR} EXCLUDE_FROM_ALL)

if(CMAKE_GENERATOR STREQUAL "Xcode")
	set_target_properties(SDL3_mixer-static PROPERTIES XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO")
endif()

set_target_properties(SDL3_mixer-static PROPERTIES FOLDER deps/sdlmixer)
