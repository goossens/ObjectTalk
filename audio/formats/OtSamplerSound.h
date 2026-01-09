//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "SDL3/SDL.h"

#include "OtAudioBuffer.h"


//
//	OtSamplerSound
//

class OtSamplerSound {
public:
	// load the specified sampler sound
	void load(const std::string& path);

	// clear the sampler sound
	inline void clear() { sound = nullptr; }

	// get/set information
	inline bool isValid() { return sound != nullptr; }
	inline OtAudioBuffer& getAudioBuffer() { return *sound; }

	inline void setRootFrequency(float rf) { rootFrequency = rf; }
	inline float getRootFrequency() { return rootFrequency; }

	// see if sampler sounds are identical
	inline bool operator==(OtSamplerSound& rhs) { return sound == rhs.sound; }
	inline bool operator!=(OtSamplerSound& rhs) { return !operator==(rhs); }

private:
	// properties
	std::shared_ptr<OtAudioBuffer> sound;
	float rootFrequency = 0.0f;
};
