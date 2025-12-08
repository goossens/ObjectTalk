//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "SDL3_mixer/SDL_mixer.h"


//
//	OtSound
//

class OtSound {
public:
	// constructors/destructor
	OtSound() = default;
	OtSound(const std::string& path) { load(path); }
	~OtSound();

	// load audio from file
	void load(const std::string& path);

	// perform actions
	void play();
	void loop();
	void pause();
	void resume();
	void stop();
	bool isPlaying();

private:
	// properties
	MIX_Audio* audio = nullptr;
	MIX_Track* track = nullptr;
};
