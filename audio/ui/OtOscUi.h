//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "nlohmann/json_fwd.hpp"

#include "OtAudioUtilities.h"
#include "OtOsc.h"


//
//	OtOscUi
//

class OtOscUi {
public:
	// UI to change oscillator properties
	bool renderUI();
	float getRenderWidth();
	float getRenderHeight();

	// (de)serialize oscillator parameters
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

	// access parameters
	inline void setParameters(OtOsc::Parameters& p) { parameters = p; }
	inline OtOsc::Parameters getParameters() { return parameters; }

private:
	// properties
	OtOsc::Parameters parameters;
};
