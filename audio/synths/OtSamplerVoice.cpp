//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "SDL3/SDL.h"

#include "OtAudioUtilities.h"
#include "OtSamplerVoice.h"


//
//	OtSamplerVoice::noteOn
//

void OtSamplerVoice::noteOn(OtSampleFile& samples, int n, int v) {
	// save parameters
	note = n;
	velocity = v;

	// set playback parameters
	active = true;
	endTime = static_cast<float>(SDL_GetTicks()) + (samples.getDurationInSeconds() * 1000.0f);
	float frequency = OtAudioUtilities::midiNoteToFrequency(note);
	dt = frequency == 0.0f ? 1.0f : frequency / samples.getRootFrequency();
	offset = 0.0f;
	volume = static_cast<float>(velocity) / 127.0f;
}


//
//	OtSamplerVoice::noteOff
//

void OtSamplerVoice::noteOff() {
}


//
//	OtSamplerVoice::cancel
//

void OtSamplerVoice::cancel() {
	active = false;
}


//
//	OtSamplerVoice::get
//

void OtSamplerVoice::get(OtSampleFile& samples, float* buffer, size_t size) {
	for (size_t i = 0; i < size; i++) {
		buffer[i] = samples.get(offset);
		offset += dt;
	}

	if (offset >= static_cast<float>(samples.getSampleCount())) {
		active = false;
	}
}
