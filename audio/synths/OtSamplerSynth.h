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

#include "OtAsset.h"

#include "OtMidiMessage.h"
#include "OtSampleFileAsset.h"
#include "OtSamplerVoice.h"


//
//	OtSamplerSynth
//

class OtSamplerSynth {
public:
	// constructor
	OtSamplerSynth();

	// configurable number of concurrent voices per synth
	static constexpr size_t numberOfVoices = 16;

	// UI to change synth properties
	bool renderUI(float itemWidth);
	float getRenderWidth();
	float getRenderHeight();

	// (de)serialize variables
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

	// process MIDI messages
	void processMidiMessage(OtMidiMessage message);

	// get the next samples
	void get(float* buffer, size_t size);

private:
	// properties
	OtAsset<OtSampleFileAsset> sampleFileAsset;

	// work variables
	std::array<OtSamplerVoice, numberOfVoices> voices;
	std::unordered_map<int, size_t> index;
	std::list<size_t> nextVoice;

	// support functions
	void noteOn(int note, int velocity);
	void noteOff(int note, int velocity);
	void AllNotesOff();

	void renderVoiceUsage(float itemWidth);
};
