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

#include "OtAdsr.h"


//
//	OtAdsrUi
//

class OtAdsrUi {
public:
	// UI to change oscillator properties
	bool renderUI();
	float getRenderWidth();
	float getRenderHeight();

	// (de)serialize oscillator parameters
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

	// access parameters
	inline void setParameters(OtAdsr::Parameters& p) { parameters = p; }
	inline void getParameters(OtAdsr::Parameters& p) { p = parameters; }

	private:
	// properties
	OtAdsr::Parameters parameters;

	// work variables
	static constexpr size_t envelopeDataSize = 1024;
	static constexpr float envelopeHeight = 100.0f;
	std::array<float, envelopeDataSize> graph;
	bool update = true;
};
