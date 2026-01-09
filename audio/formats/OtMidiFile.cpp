//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "SDL3/SDL.h"

#include "OtPath.h"
#include "OtLog.h"

#include "OtMidiFile.h"


//
//	OtMidiFile::load
//

void OtMidiFile::load(const std::string& path) {
	midi = std::make_shared<smf::MidiFile>();

	// sanity checks
	if (!OtPath::exists(path)) {
		midi = nullptr;
		OtLogError("Can't open MIDI midi [{}]", path);
	}

	if (!midi->read(path)) {
		midi = nullptr;
		OtLogError("Can't load MIDI midi [{}]", path);
	}

	// build a list of relevant events
	midi->joinTracks();
	events.clear();

	for (auto i = 0; i < getEventCount(0); i++) {
		if (isNote(0, i)) {
			events.emplace_back(getTimeInSeconds(0, i), &(*midi)[0][i]);
		}
	}
}


//
//	OtMidiFile::start
//

void OtMidiFile::start() {
	if (events.size()) {
		currentPlay = 0.0f;
		playEnd = static_cast<float>(midi->getFileDurationInSeconds());
		nextEvent = 0;
		lastEventTime = SDL_GetTicks();
		playingFlag = true;
		pausingFlag = false;
		loopingFlag = false;
	}
}


//
//	OtMidiFile::loop
//

void OtMidiFile::loop() {
	if (events.size()) {
		currentPlay = 0.0f;
		playEnd = static_cast<float>(midi->getFileDurationInSeconds());
		nextEvent = 0;
		lastEventTime = SDL_GetTicks();
		playingFlag = true;
		pausingFlag = false;
		loopingFlag = true;
	}
}


//
//	OtMidiFile::stop
//

void OtMidiFile::stop() {
	currentPlay = 0.0f;
	playEnd = 0.0f;
	nextEvent = 0;
	lastEventTime = 0;
	playingFlag = false;
	pausingFlag = false;
	loopingFlag = false;
}


//
//	OtMidiFile::pause
//

void OtMidiFile::pause() {
	if (playingFlag) {
		playingFlag = false;
		pausingFlag = true;
	}
}


//
//	OtMidiFile::resume
//

void OtMidiFile::resume() {
	if (pausingFlag) {
		lastEventTime = SDL_GetTicks();
		playingFlag = true;
		pausingFlag = false;
	}
}


//
//	OtMidiFile::process
//

void OtMidiFile::process(std::function<void(bool, int, int)> callback) {
	if (playingFlag) {
		// determine current play time by adding interval since last run
		auto now = SDL_GetTicks();
		currentPlay += static_cast<float>(now - lastEventTime) / 1000.0f;
		lastEventTime = now;

		// process all events that are due
		while (playingFlag && events[nextEvent].time < currentPlay) {
			auto& midiEvent = *events[nextEvent].event;
			callback(midiEvent.isNoteOn(), midiEvent.getKeyNumber(), midiEvent.getVelocity());

			// if we reach the end of the MIDI stream, either loop or stop
			if (++nextEvent == events.size()) {
				if (loopingFlag) {
					currentPlay = 0.0f;
					nextEvent = 0;

				} else {
					stop();
				}
			}
		}
	}
}


//
//	OtMidiFile::display
//

void OtMidiFile::display() {
	for (auto& event : events) {
		OtLogDebug(
			"{}, {}, {}, {}",
			event.time,
			event.event->isNoteOn() ? "On" : "Off",
			event.event->getKeyNumber(),
			event.event->getVelocity());
	}
}
