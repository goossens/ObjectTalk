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

#include "OtVco.h"


//
//	OtVcoUi
//

class OtVcoUi {
public:
	// UI to change properties
	inline void showFrequency(bool flag) { showingFrequency = flag; }
	bool renderUI();
	float getRenderWidth();
	float getRenderHeight();

	// (de)serialize parameters
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

	// access parameters
	inline void setParameters(OtVco::Parameters& p) { parameters = p; }
	inline OtVco::Parameters getParameters() { return parameters; }
	inline float getFrequency() { return frequency; }

private:
	// properties
	OtVco::Parameters parameters;
	float frequency = 440.0f;
	bool showingFrequency = false;
};
