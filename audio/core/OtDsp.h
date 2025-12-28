//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtSignalBuffer.h"


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
	inline void setSignalProvider(std::function<void(OtSignalBuffer&)> p) { provider = p; };

private:
	// properties
	SDL_AudioStream* stream = nullptr;
	MIX_Track* track = nullptr;
	OtSignalBuffer buffer{2};

	std::function<void(OtSignalBuffer&)> provider = nullptr;

	// provide more data to the stream
	void getStreamData(int additional, int total);
};
