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
#include "OtAnalogVoice.h"


//
//	OtAnalogVoice::noteOn
//

void OtAnalogVoice::noteOn(int n, int v) {
	instanceClear();
	gate = true;
	note = n;
	velocity = v;
	frequency = OtAudioUtilities::midiNoteToPitch(note);
	active = true;
}


//
//	OtAnalogVoice::noteOff
//

void OtAnalogVoice::noteOff() {
	endTime = static_cast<float>(SDL_GetTicks()) + (getVcaRelease() * 1000.0f);
	gate = false;
}


//
//	OtAnalogVoice::cancel
//

void OtAnalogVoice::cancel() {
	instanceClear();
	gate = false;
	endTime = 0;
	active = false;
}


//
//	OtAnalogVoice::get
//

void OtAnalogVoice::get(Parameters& parameters, float* buffer, size_t size) {
	setParameters(parameters);
	setVcoFrequency1(frequency);
	setVcoFrequency2(frequency);
	setVcfGate(static_cast<float>(gate));
	setVcaGate(static_cast<float>(gate));

	float* output[] = {buffer};
	compute(static_cast<int>(size), nullptr, output);

	if (!gate && static_cast<float>(SDL_GetTicks()) > endTime) {
		endTime = 0;
		active = false;
	}
}
