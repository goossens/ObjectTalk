//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <numeric>

#include "OtSynth.h"


//
//	OtSynth::OtSynth
//

OtSynth::OtSynth() {
	nextVoice.resize(numberOfVoices);
	std::iota(nextVoice.begin(), nextVoice.end(), 0);
}


//
//	OtSynth::processMidiMessage
//

void OtSynth::processMidiMessage(OtMidiMessage message) {
	if (message->isNoteOn()) {
		noteOn(message->getKeyNumber(), message->getVelocity());

	} else if (message->isNoteOff()) {
		noteOff(message->getKeyNumber(), message->getVelocity());

	} else if (message->isAllNotesOff()) {
		AllNotesOff();
	}
}

float OtSynth::get() {
	auto sample = 0.0f;

	for (auto& voice : voices) {
		if (voice.isActive()) {
			sample += voice.get(parameters);
		}
	}

	return sample;
}


//
//	OtSynth::get
//

void OtSynth::get(float* buffer, size_t size) {
	for (size_t i = 0; i < size; i++) {
		*buffer++ = get();
	}
}


//
//	OtSynth::noteOn
//

void OtSynth::noteOn(int note, int velocity) {
	// get next available voice
	auto openVoice = nextVoice.end();

	for (auto i = nextVoice.begin(); i != nextVoice.end() && openVoice == nextVoice.end(); i++) {
		if (!voices[*i].isActive()) {
			openVoice = i;
		}
	}

	// if we ran out of voices, we steal the oldest voice
	if (openVoice == nextVoice.end()) {
		openVoice = nextVoice.begin();
		voices[*openVoice].cancel(parameters);
	}

	// put voice at the end of the list and add index entry
	nextVoice.splice(nextVoice.end(), nextVoice, openVoice);
	index[note] = *openVoice;

	// turn voice on
	voices[*openVoice].noteOn(parameters, note, velocity);
}


//
//	OtSynth::noteOff
//

void OtSynth::noteOff([[maybe_unused]] int note, [[maybe_unused]] int velocity) {
	voices[index[note]].noteOff(parameters);
}


//
//	OtSynth::AllNotesOff
//

void OtSynth::AllNotesOff() {
	for (auto& voice : voices) {
		if (voice.isActive()) {
			noteOff(voice.getNote(), 0);
		}
	}
}
