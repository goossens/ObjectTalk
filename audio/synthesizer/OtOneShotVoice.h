//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtOneShotFileAsset.h"
#include "OtSynthVoice.h"


//
//	OtSamplerVoice
//

class OtSamplerVoice : public OtSynthVoice {
public:
	// start/stop playing the specified MIDI note
	void start(int note, int velocity, int pitchWheel) override;
	void stop(int velocity) override;
	bool isActive() override;

private:
	OtOneShotFileAsset asset;
};
