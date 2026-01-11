//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtAudioUi.h"
#include "OtSynthSound.h"


//
//	OtSynthSound::renderUI
//

bool OtSynthSound::renderUI() {
	bool changed = false;
	changed |= renderSound();
	changed |= renderEnvelope();
	return changed;
}


//
//	OtSynthSound::serialize
//

void OtSynthSound::serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	(*data)["attack"] = attack;
	(*data)["hold"] = hold;
	(*data)["decay"] = decay;
	(*data)["sustain"] = sustain;
	(*data)["release"] = release;
}


//
//	OtSynthSound::deserialize
//

void OtSynthSound::deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	attack = data->value("attack", 1.0f);
	hold = data->value("hold", 0.0f);
	decay = data->value("decay", 1.0f);
	sustain = data->value("sustain", 0.5f);
	release = data->value("release", 0.2f);
	updateEnvelope = true;
}


//
//	OtSynthSound::renderEnvelope
//

bool OtSynthSound::renderEnvelope() {
	bool changed = false;
	changed |= OtAudioUi::envelope(&attack, &hold, &decay, &sustain, &release, &envelopeData, updateEnvelope);
	updateEnvelope = changed;
	return changed;
}
