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

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"

#include "OtAudioBuffer.h"


//
//	OtDsp
//

class OtDsp {
public:
	// constructor/destructor
	OtDsp();
	~OtDsp();

	// perform actions
	void start();
	void stop();
	bool isPlaying();

	void setGain(float gain);
	float getGain();

	// set signal provider
	inline void setSignalProvider(std::function<void(OtAudioBuffer&)> p) { provider = p; };

private:
	// properties
	SDL_AudioStream* stream = nullptr;
	MIX_Track* track = nullptr;
	OtAudioBuffer buffer{2};

	std::function<void(OtAudioBuffer&)> provider = nullptr;

	// provide more data to the stream
	void getStreamData(int additional, int total);
};
