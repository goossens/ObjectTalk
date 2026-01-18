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


//
//	OtSynth::noteOn
//

void OtSynth::noteOn(int note, int velocity) {
	if (index.count(note)) {
		voices[index[note]].cancel();
	}

	auto voice = nextVoice.front();
	nextVoice.splice(nextVoice.end(), nextVoice, nextVoice.begin());

	voices[voice].noteOn(&parameters, note, velocity);
}


//
//	OtSynth::noteOff
//

void OtSynth::noteOff(int note, int velocity) {
	voices[index[note]].noteOff(note, velocity);
}


//
//	OtSynth::AllNotesOff
//

void OtSynth::AllNotesOff() {
	for (auto& voice : voices) {
		if (voice.isActive()) {
			noteOff(voice.note, 0);
		}
	}
}
