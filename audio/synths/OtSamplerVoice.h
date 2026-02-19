//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSampleFile.h"


//
//	OtSamplerVoice
//

class OtSamplerVoice {
public:
	// voice state
	void noteOn(OtSampleFile& samples, int note, int velocity);
	void noteOff();
	void cancel();

	inline bool isActive() { return active; }
	inline int getNote() { return note; }
	inline int getVelocity() { return velocity; }

	// get the next sample(s)
	void get(OtSampleFile& samples, float* buffer, size_t size);

private:
	int note;
	int velocity;

	bool active = false;
	float endTime;
	float dt;
	float offset;
	float volume;
};
