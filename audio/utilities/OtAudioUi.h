//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>


//
//	OtAudioUi
//

class OtAudioUi {
public:
	// UI to edit an envelope
	static bool envelope(float* attack, float* hold, float* decay, float* sustain, float* release, std::vector<float>* values, bool update);
	static float getEnvelopeWidth();
	static float getEnvelopeHeight();
};
