//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <memory>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtSynthSound.h"
#include "OtSynthSoundFactory.h"


//
//	OtSynth
//

class OtSynth {
public:
	// add a sound to this synth
	inline void addSound(std::shared_ptr<OtSynthSound> sound) {
		sounds.emplace_back(sound);
	}

	// remove a sound from this synth
	inline void removeSound(std::shared_ptr<OtSynthSound> sound) {
		sounds.erase(std::remove(sounds.begin(), sounds.end(), sound), sounds.end());
	}

	// UI to change sound properties
	bool renderUI();

	// (de)serialize component
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

private:
	// properties
	OtSynthSoundFactory factory;
	std::vector<std::shared_ptr<OtSynthSound>> sounds;
	size_t maxVoices = 8;
};
