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

#include "SDL3/SDL.h"

#include "OtMidiMessage.h"


//
//	OtMidiFile
//

class OtMidiFile {
public:
	// load the specified MIDI midi
	void load(const std::string& path);

	// clear the MIDI midi
	void clear();

	// get information
	inline bool isValid() { return events != nullptr; }
	inline size_t getMessageCount() { return events->size(); }
	inline float getMessageTime(size_t index) { return (*events)[index].time; }
	inline std::shared_ptr<OtMidiMessage> getMessage(size_t index) { return (*events)[index].message; }

	// playback control
	void start();
	void stop();
	void pause();
	void resume();
	inline void setLooping(bool looping) {loopingFlag = looping; }

	// get playback status
	inline bool isPlaying() { return playingFlag; }
	inline bool isLooping() { return loopingFlag; }
	inline bool isPausing() { return pausingFlag; }

	// process next event (if required)
	// callbacks are called for relevant events
	void process(std::function<void(std::shared_ptr<OtMidiMessage>)> callback);

	// see if MIDI files are identical
	inline bool operator==(OtMidiFile& rhs) { return events == rhs.events; }
	inline bool operator!=(OtMidiFile& rhs) { return !operator==(rhs); }

private:
	// list of events with timestamps
	struct Event {
		Event(float t, uint8_t* msg, size_t size) {
			time = t;
			message = std::make_shared<OtMidiMessage>(msg, size);
		}

		std::shared_ptr<OtMidiMessage> message;
		float time;
	};

	std::shared_ptr<std::vector<Event>> events;

	// play parameters
	float currentPlay;
	float playEnd;
	size_t nextEvent;
	Uint64 lastEventTime;
	bool playingFlag = false;
	bool pausingFlag = false;
	bool loopingFlag = false;
	bool sendAllNotesOff = false;
};
