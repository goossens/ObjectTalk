//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"


//
//	OtSynthVoice
//

class OtSynthVoice {
public:
	// destructor
	virtual ~OtSynthVoice() {}

	// UI to change component properties
	virtual bool renderUI() = 0;

	// (de)serialize component
	virtual nlohmann::json serialize(std::string* basedir) = 0;
	virtual void deserialize(nlohmann::json data, std::string* basedir) = 0;

	// start/stop playing the specified MIDI note
	virtual void noteOn(int note, int velocity, int pitchWheel) = 0;
	virtual void noteOff(int velocity) = 0;
	virtual bool isActive() = 0;

	// MIDI events during playback
	virtual void pitchWheelMoved ([[maybe_unused]] int value) {}
	virtual void controllerMoved ([[maybe_unused]] int controller, [[maybe_unused]] int value) {}
	virtual void afterTouchChanged ([[maybe_unused]] int value) {}
	virtual void channelPressureChanged ([[maybe_unused]] int value) {}
};
