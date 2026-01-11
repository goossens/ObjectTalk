//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtSynth.h"


//
//	OtSynth::renderUI
//

bool OtSynth::renderUI() {
	bool changed = OtUi::dragSizeT("Max Voices", &maxVoices, 1, 16);
	return changed;
}


//
//	OtSynth::serialize
//

void OtSynth::serialize(nlohmann::json* data, std::string* basedir) {
	(*data)["maxVoices"] = maxVoices;
	auto soundsArray = nlohmann::json::array();

	for (auto& sound : sounds) {
		auto soundData = nlohmann::json::object();
		soundData["type"] = sound->name;
		sound->serialize(&soundData, basedir);
		soundsArray.emplace_back(soundData);
	}

	(*data)["sounds"] = soundsArray;
}


//
//	OtSynth::deserialize
//

void OtSynth::deserialize(nlohmann::json* data, std::string* basedir) {
	maxVoices = data->value("maxVoices", 8);

	for (auto& soundData : (*data)["sounds"]) {
		if (soundData.contains("type")) {
			std::string soundType = soundData["type"];
			auto sound = factory.create(soundType);
			sound->deserialize(&soundData, basedir);
			addSound(sound);
		}
	}
}
