//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtMixer.h"
#include "OtSound.h"


//
//	OtSound::~OtSound
//

OtSound::~OtSound() {
	if (audio) {
		MIX_DestroyAudio(audio);
		audio = nullptr;
	}

	if (track) {
		MIX_DestroyTrack(track);
		track = nullptr;
	}
}


//
//	OtSound::load
//

void OtSound::load(const std::string& path) {
	// cleanup first (if required)
	if (audio) {
		MIX_DestroyAudio(audio);
		audio = nullptr;
	}

	if (track) {
		MIX_DestroyTrack(track);
		track = nullptr;
	}

	// load the sound file
	auto mixer = OtMixer::instance().getRawMixer();
	audio = MIX_LoadAudio(mixer, path.c_str(), true);

	if (audio == nullptr) {
		OtLogError("Can't load audio file [{}]: {}", path, SDL_GetError());
	}

	// create a track for the sound file
	track = MIX_CreateTrack(mixer);

	if (!track) {
		OtLogFatal("Error in MIX_CreateTrack: {}", SDL_GetError());
	}

	if (!MIX_SetTrackAudio(track, audio)) {
		OtLogFatal("Error in MIX_SetTrackAudio: {}", SDL_GetError());
	}
}


//
//	OtSound::play
//

void OtSound::play() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	// play the track
	if (!MIX_PlayTrack(track, 0)) {
		OtLogFatal("Error in MIX_PlayTrack: {}", SDL_GetError());
	}
}


//
//	OtSound::loop
//

void OtSound::loop() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	// loop the track
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);

	if (!MIX_PlayTrack(track, props)) {
		OtLogFatal("Error in MIX_PlayTrack: {}", SDL_GetError());
	}

	SDL_DestroyProperties(props);
}


//
//	OtSound::pause
//

void OtSound::pause() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	// pause the track
	if (!MIX_PauseTrack(track)) {
		OtLogFatal("Error in MIX_PauseTrack: {}", SDL_GetError());
	}
}


//
//	OtSound::resume
//

void OtSound::resume() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	// pause the track
	if (!MIX_ResumeTrack(track)) {
		OtLogFatal("Error in MIX_ResumeTrack: {}", SDL_GetError());
	}
}


//
//	OtSound::stop
//

void OtSound::stop() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	// pause the track
	if (!MIX_StopTrack(track, 0)) {
		OtLogFatal("Error in MIX_StopTrack: {}", SDL_GetError());
	}
}


//
//	OtSound::isPlaying
//

bool OtSound::isPlaying() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	return MIX_TrackPlaying(track);
}


//
//	OtSound::setGain
//

void OtSound::setGain(float gain) {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	if (!MIX_SetTrackGain(track, gain)) {
		OtLogFatal("Error in MIX_SetTrackGain: {}", SDL_GetError());
	}
}


//
//	OtSound::getGain
//

float OtSound::getGain() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	return MIX_GetTrackGain(track);
}
