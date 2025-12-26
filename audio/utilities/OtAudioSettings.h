//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>

#include "OtNumbers.h"


//
//	OtAudioSettings
//

class OtAudioSettings {
public:
	static constexpr size_t bufferSize = 32;
	static constexpr size_t sampleRate = 48000;
	static constexpr float dt = 1.0f / static_cast<float>(sampleRate);
	static constexpr float pi = static_cast<float>(std::numbers::pi);
};

