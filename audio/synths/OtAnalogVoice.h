//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtVoice.h"


//
//	OtAnalogVoice
//

class OtAnalogVoice : public OtVoice {
public:
	// voice state
	void noteOn(int note, int velocity);
	void noteOff();
	void cancel();

	inline bool isActive() { return active; }
	inline int getNote() { return note; }
	inline int getVelocity() { return velocity; }

	// get the next sample(s)
	void get(Parameters& parameters, float* buffer, size_t size);

private:
	// properties
	bool gate = false;
	float endTime = 0;
	int note;
	int velocity;
	float frequency;
	bool active = false;
};
