//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "MidiFile.h"


//
//	OtMidiFile
//

class OtMidiFile {
public:
	// load the specified MIDI midi
	void load(const std::string& path);

	// clear the MIDI midi
	inline void clear() { midi = nullptr; }

	// get information
	inline bool isValid() { return midi != nullptr; }
	inline int getTrackCount() { return midi->getTrackCount(); }
	inline int getEventCount(int track) { return (*midi)[track].getEventCount(); }

	// access events
	inline float getTimeInSeconds(int track, int event) { return static_cast<float>(midi->getTimeInSeconds(track, event)); }
	inline bool isNote(int track, int event) { return (*midi)[track][event].isNote(); }
	inline bool isNoteOn(int track, int event) { return (*midi)[track][event].isNoteOn(); }
	inline bool isNoteOff(int track, int event) { return (*midi)[track][event].isNoteOff(); }
	inline int getKeyNumber(int track, int event) { return (*midi)[track][event].getKeyNumber(); }
	inline int getVelocity(int track, int event) { return (*midi)[track][event].getVelocity(); }

	// playback control
	void start();
	void loop();
	void stop();
	void pause();
	void resume();

	// get playback status
	inline bool isPlaying() { return playingFlag; }
	inline bool isLooping() { return loopingFlag; }
	inline bool isPaused() { return pausingFlag; }

	// process next event (if required)
	// callbacks are called for relevant events
	void process(std::function<void(bool, int, int)> callback);

	// display events list (for debugging only)
	void display();

	// see if MIDI files are identical
	inline bool operator==(OtMidiFile& rhs) { return midi == rhs.midi; }
	inline bool operator!=(OtMidiFile& rhs) { return !operator==(rhs); }

private:
	// MIDI data from file
	std::shared_ptr<smf::MidiFile> midi;

	// list of relevant events with timestamps
	struct Event {
		Event(float t, smf::MidiEvent* e) : time(t), event(e) {}
		float time;
		smf::MidiEvent* event;
	};

	std::vector<Event> events;

	// play parameters
	float currentPlay = 0.0f;
	float playEnd = 0.0f;
	size_t nextEvent;
	Uint64 lastEventTime = 0;
	bool playingFlag = false;
	bool pausingFlag = false;
	bool loopingFlag = false;
};
