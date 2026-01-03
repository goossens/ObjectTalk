//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtFrameworkAtExit.h"

#include "OtMixer.h"


//
//	OtMixer::getRawMixer
//

MIX_Mixer* OtMixer::getRawMixer() {
	// initialize library (if required)
	if (!mixerLibraryInitialized) {
		if (!MIX_Init()) {
			OtLogFatal("Error in MIX_Init: {}", SDL_GetError());
		}

		mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

		if (!mixer) {
			OtLogFatal("Error in MIX_CreateMixerDevice: {}", SDL_GetError());
		}

		OtFrameworkAtExit::add([this]() {
			MIX_DestroyMixer(mixer);
			MIX_Quit();
		});

		mixerLibraryInitialized = true;
	}

	return mixer;
}
