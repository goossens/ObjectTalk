//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "MidiFile.h"

#include "OtPath.h"
#include "OtLog.h"

#include "OtMidiFile.h"


//
//	OtMidiFile::load
//

void OtMidiFile::load(const std::string& path) {
	stop();
	messages = std::make_shared<std::vector<Event>>();

	smf::MidiFile midi;

	// sanity checks
	if (!OtPath::exists(path)) {
		OtLogError("Can't open MIDI midi [{}]", path);
	}

	if (!midi.read(path)) {
		OtLogError("Can't load MIDI midi [{}]", path);
	}

	// build a list of relevant messages
	midi.joinTracks();

	for (auto i = 0; i < midi[0].getEventCount(); i++) {
		auto& event = midi[0][i];

		messages->emplace_back(
			static_cast<float>(midi.getTimeInSeconds(0, i)),
			reinterpret_cast<uint8_t*>(event.data()), event.size());
	}

	playEnd = static_cast<float>(midi.getFileDurationInSeconds());
}

void OtMidiFile::clear() {
	if (playingFlag || pausingFlag) {
		stop();
		sendAllNotesOff = true;
	}

	messages = nullptr;
}


//
//	OtMidiFile::start
//

void OtMidiFile::start() {
	if (messages->size()) {
		currentPlay = 0.0f;
		nextEvent = 0;
		lastEventTime = SDL_GetTicks();
		playingFlag = true;
		pausingFlag = false;
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
	sendAllNotesOff = true;
}


//
//	OtMidiFile::pause
//

void OtMidiFile::pause() {
	if (playingFlag) {
		playingFlag = false;
		pausingFlag = true;
		sendAllNotesOff = true;
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

void OtMidiFile::process(std::function<void(std::shared_ptr<OtMidiMessage>)> callback) {
	if (messages && playingFlag) {
		// determine current play time by adding interval since last run
		auto now = SDL_GetTicks();
		currentPlay += static_cast<float>(now - lastEventTime) / 1000.0f;
		lastEventTime = now;

		// process all messages that are due
		while (playingFlag && messages->at(nextEvent).time < currentPlay) {
			callback((*messages)[nextEvent].message);

			// if we reach the end of the MIDI stream, either loop or stop
			if (++nextEvent == messages->size()) {
				if (loopingFlag) {
					currentPlay = 0.0f;
					nextEvent = 0;

				} else {
					stop();
				}
			}
		}

	} else if (sendAllNotesOff) {
		auto message = std::make_shared<OtMidiMessage>();
		message->makeAllNotesOff(0);
		callback(message);
		sendAllNotesOff = false;
	}
}
