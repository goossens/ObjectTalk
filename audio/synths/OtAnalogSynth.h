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

#include "OtAnalogVoice.h"
#include "OtAudioUi.h"
#include "OtMidiMessage.h"


//
//	OtAnalogSynth
//

class OtAnalogSynth : public OtVoice {
public:
	// constructor
	OtAnalogSynth();

	// configurable number of concurrent voices per synth
	static constexpr size_t numberOfVoices = 16;

	// UI to change synth properties
	bool renderUI(float itemWidth);

	// process MIDI messages
	void processMidiMessage(OtMidiMessage message);

	// get the next samples
	void get(float* buffer, size_t size);

private:
	// properties
	std::array<OtAnalogVoice, numberOfVoices> voices;
	std::unordered_map<int, size_t> index;
	std::list<size_t> nextVoice;
	OtAudioUi::AdsrState vcfAdsrState;
	OtAudioUi::AdsrState vcaAdsrState;
	static constexpr float envelopeHeight = 100.0f;

	// support functions
	void noteOn(int note, int velocity);
	void noteOff(int note, int velocity);
	void AllNotesOff();

	bool renderVco(float width, float height);
	bool renderVcf(float width, float height);
	bool renderVca(float width, float height);
};
