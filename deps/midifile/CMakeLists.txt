#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(midifile QUIET GIT_REPOSITORY "https://github.com/craigsapp/midifile")

set(MIDIFILE_SOURCES
	${midifile_SOURCE_DIR}/src/Binasc.cpp
	${midifile_SOURCE_DIR}/src/MidiEvent.cpp
	${midifile_SOURCE_DIR}/src/MidiEventList.cpp
	${midifile_SOURCE_DIR}/src/MidiFile.cpp
	${midifile_SOURCE_DIR}/src/MidiMessage.cpp
	${midifile_SOURCE_DIR}/include/Binasc.h
	${midifile_SOURCE_DIR}/include/MidiEvent.h
	${midifile_SOURCE_DIR}/include/MidiEventList.h
	${midifile_SOURCE_DIR}/include/MidiFile.h
	${midifile_SOURCE_DIR}/include/MidiMessage.h)

add_library(midifile STATIC ${MIDIFILE_SOURCES})
target_include_directories(midifile PUBLIC ${midifile_SOURCE_DIR}/include)

set_target_properties(midifile PROPERTIES FOLDER deps)
