//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtLog.h"

#include "OtAudioSettings.h"

#include "OtSampleFile.h"


//
//	OtSampleFile::load
//

void OtSampleFile::load(const std::string& path) {
	SDL_AudioSpec spec;
	Uint8* buffer;
	Uint32 len;

	// try to load the data
	if (!SDL_LoadWAV(path.c_str(), &spec, &buffer, &len)) {
		OtLogError("Can't open sample file [{}], error: {}", path, SDL_GetError());
	}

	// convert format (if required)
	if (spec.format != SDL_AUDIO_F32 || spec.freq != static_cast<int>(OtAudioSettings::sampleRate)) {
		SDL_AudioSpec newSpec;
		Uint8* newBuffer;
		Uint32 newLen;

		newSpec.format = SDL_AUDIO_F32;
		newSpec.channels = spec.channels;
		newSpec.freq = static_cast<int>(OtAudioSettings::sampleRate);

		if (!SDL_ConvertAudioSamples(&spec, buffer, static_cast<int>(len), &newSpec, &newBuffer, reinterpret_cast<int*>(&newLen))) {
			SDL_free(buffer);
			OtLogError("Can't convert sample file [{}], error: {}", path, SDL_GetError());
		}

		SDL_free(buffer);
		spec = newSpec;
		buffer = newBuffer;
		len = newLen;
	}

	// now turn it into an audio buffer
	auto sampleCount = static_cast<size_t>(len / 4);
	sound = std::make_shared<OtAudioBuffer>(static_cast<size_t>(spec.channels), sampleCount);
	std::copy(reinterpret_cast<float*>(buffer), reinterpret_cast<float*>(buffer) + sampleCount, sound->data());
	SDL_free(buffer);
}
