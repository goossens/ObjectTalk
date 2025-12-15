//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "SDL3_mixer/SDL_mixer.h"

#include "OtSingleton.h"


//
//	OtMixer
//

class OtMixer : public OtSingleton<OtMixer> {
public:
	// access raw mixer
	MIX_Mixer* getRawMixer();

	// return the sample rate for the mixer's output device
	size_t getSampleRate();

private:
	bool mixerLibraryInitialized = false;
	MIX_Mixer* mixer = nullptr;
};
