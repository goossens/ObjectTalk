//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtAudioSettings.h"
#include "OtMixerInput.h"
#include "OtMixer.h"


//
//	OtMixerInput::OtMixerInput
//

OtMixerInput::OtMixerInput() {
	// get mixer information
	auto& mixer = OtMixer::instance();

	// create an audio stream
	SDL_AudioSpec src;
	src.format = SDL_AUDIO_F32;
	src.channels = 2;
	src.freq = static_cast<int>(OtAudioSettings::sampleRate);

	SDL_AudioSpec dst;
	dst.format = SDL_AUDIO_F32;
	dst.channels = 2;
	dst.freq = static_cast<int>(OtAudioSettings::sampleRate);

	stream = SDL_CreateAudioStream(&src, &dst);

	if (!stream) {
		OtLogFatal("Error in SDL_CreateAudioStream: {}", SDL_GetError());
	}

	// create a track
	track = MIX_CreateTrack(mixer.getRawMixer());

	if (!track) {
		OtLogFatal("Error in MIX_CreateTrack: {}", SDL_GetError());
	}

	if (!MIX_SetTrackAudioStream(track, stream)) {
		OtLogFatal("Error in MIX_SetTrackAudioStream: {}", SDL_GetError());
	}

	// set the callback to collect stream data
	bool success = SDL_SetAudioStreamGetCallback(stream, [](void* data, SDL_AudioStream*, int additional, int total) {
		((OtMixerInput*) data)->getStreamData(additional, total);
	}, this);

	if (!success) {
		OtLogFatal("Error in SDL_SetAudioStreamGetCallback: {}", SDL_GetError());
	}

	// associate stream with track
	if (!MIX_SetTrackAudioStream(track, stream)) {
		OtLogFatal("Error in MIX_SetTrackAudioStream: {}", SDL_GetError());
	}
}


//
//	OtMixerInput::~OtMixerInput
//

OtMixerInput::~OtMixerInput() {
	// stop track (if required)
	if (isPlaying()) {
		stop();
	}

	if (track) {
		MIX_DestroyTrack(track);
	}

	if (stream) {
		SDL_DestroyAudioStream(stream);
	}
}


//
//	OtMixerInput::start
//

void OtMixerInput::start() {
	// play the track
	if (!MIX_PlayTrack(track, 0)) {
		OtLogFatal("Error in MIX_PlayTrack: {}", SDL_GetError());
	}
}


//
//	OtMixerInput::stop
//

void OtMixerInput::stop() {
	// stop the track
	if (!MIX_StopTrack(track, 0)) {
		OtLogFatal("Error in MIX_StopTrack: {}", SDL_GetError());
	}
}


//
//	OtMixerInput::isPlaying
//

bool OtMixerInput::isPlaying() {
	return MIX_TrackPlaying(track);
}


//
//	OtMixerInput::setGain
//

void OtMixerInput::setGain(float gain) {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	if (!MIX_SetTrackGain(track, gain)) {
		OtLogFatal("Error in MIX_SetTrackGain: {}", SDL_GetError());
	}
}


//
//	OtMixerInput::getGain
//

float OtMixerInput::getGain() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	return MIX_GetTrackGain(track);
}


//
//	OtMixerInput::getStreamData
//

void OtMixerInput::getStreamData([[maybe_unused]] int additional, [[maybe_unused]] int total) {
	if (isPlaying() && provider) {
		provider(buffer);

	} else {
		buffer.clear(0.0f);
	}

	if (!SDL_PutAudioStreamData(stream, buffer.data(), static_cast<int>(OtAudioSettings::bufferSize * sizeof(float) * 2))) {
		OtLogFatal("Error in SDL_PutAudioStreamData: {}", SDL_GetError());
	}
}
