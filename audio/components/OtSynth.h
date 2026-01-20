//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <list>
#include <string>
#include <unordered_map>

#include "nlohmann/json_fwd.hpp"

#include "OtMidiMessage.h"
#include "OtVoice.h"

//
//	OtSynth
//

class OtSynth {
public:
	// constructor
	OtSynth();

	// configurable number of concurrent voice per synth
	static constexpr size_t numberOfVoices = 16;

	// UI to change synth properties
	inline bool renderUI(float itemWidth) { return parameters.renderUI(itemWidth); }
	inline float getRenderWidth() { return parameters.getRenderWidth(); }
	inline float getRenderHeight() { return parameters.getRenderHeight(); }

	// (de)serialize data
	inline void serialize(nlohmann::json* data, std::string* basedir) { parameters.serialize(data, basedir); }
	inline void deserialize(nlohmann::json* data, std::string* basedir) { parameters.deserialize(data, basedir); }

	// process MIDI messages
	void processMidiMessage(OtMidiMessage message);

	// get the next sample(s)
	float get();
	void get(float* buffer, size_t size);

private:
	// properties
	OtVoice::Parameters parameters;
	std::array<OtVoice::State, numberOfVoices> voices;
	std::unordered_map<int, size_t> index;
	std::list<size_t> nextVoice;

	// support functions
	void noteOn(int note, int velocity);
	void noteOff(int note, int velocity);
	void AllNotesOff();
};
