//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtAudioSettings.h"
#include "OtDsp.h"
#include "OtMixer.h"


//
//	OtDsp::OtDsp
//

OtDsp::OtDsp() {
	// get mixer information
	auto& mixer = OtMixer::instance();

	// create an audio stream
	SDL_AudioSpec src;
	src.format = SDL_AUDIO_F32;
	src.channels = 2;
	src.freq = OtAudioSettings::sampleRate;

	SDL_AudioSpec dst;
	dst.format = SDL_AUDIO_F32;
	dst.channels = 2;
	dst.freq = OtAudioSettings::sampleRate;

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
		((OtDsp*) data)->getStreamData(additional, total);
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
//	OtDsp::~OtDsp
//

OtDsp::~OtDsp() {
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
//	OtDsp::start
//

void OtDsp::start() {
	// play the track
	if (!MIX_PlayTrack(track, 0)) {
		OtLogFatal("Error in MIX_PlayTrack: {}", SDL_GetError());
	}
}


//
//	OtDsp::stop
//

void OtDsp::stop() {
	// stop the track
	if (!MIX_StopTrack(track, 0)) {
		OtLogFatal("Error in MIX_StopTrack: {}", SDL_GetError());
	}
}


//
//	OtDsp::isPlaying
//

bool OtDsp::isPlaying() {
	return MIX_TrackPlaying(track);
}


//
//	OtDsp::setGain
//

void OtDsp::setGain(float gain) {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	if (!MIX_SetTrackGain(track, gain)) {
		OtLogFatal("Error in MIX_SetTrackGain: {}", SDL_GetError());
	}
}


//
//	OtDsp::getGain
//

float OtDsp::getGain() {
	// sanity check
	if (!track) {
		OtLogFatal("Invalid operation on uninitialized sound");
	}

	return MIX_GetTrackGain(track);
}


//
//	OtDsp::getStreamData
//

void OtDsp::getStreamData([[maybe_unused]] int additional, [[maybe_unused]] int total) {
	if (isPlaying() && provider) {
		provider(buffer);

	} else {
		buffer.clear(0.0f);
	}

	if (!SDL_PutAudioStreamData(stream, buffer.data(), static_cast<int>(OtAudioSettings::bufferSize * sizeof(float) * 2))) {
		OtLogFatal("Error in SDL_PutAudioStreamData: {}", SDL_GetError());
	}
}
