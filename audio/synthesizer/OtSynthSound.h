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

#include "nlohmann/json_fwd.hpp"


//
//	OtSynthSound
//

class OtSynthSound {
public:
	// destructor
	virtual ~OtSynthSound() {}

	// see if sound is ready to be used
	virtual bool isReady() { return false; }

	// UI to change sound properties
	virtual bool renderUI();

	// (de)serialize component
	virtual void serialize(nlohmann::json* data, std::string* basedir);
	virtual void deserialize(nlohmann::json* data, std::string* basedir);

	// get sound type name
	virtual const char* getTypeName() { return name; }
	static constexpr char const* name = "";

protected:
	// local support functions
	virtual bool renderSound() { return false; }
	virtual bool renderEnvelope();

private:
	// properties
	float attack = 1.0f;
	float hold = 0.0f;
	float decay = 1.0f;
	float sustain = 0.8f;
	float release = 0.2f;

	// work variables
	std::vector<float> envelopeData;
	bool updateEnvelope = true;
};
