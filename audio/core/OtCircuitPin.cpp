//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtUi.h"

#include "OtCircuit.h"
#include "OtCircuitPin.h"


//
//	OtCircuitPinClass::getSample
//

float OtCircuitPinClass::getSample(size_t channel, size_t sample) {
	float value = sourcePin->audioBuffer->get(channel, sample);

	if (attenuationFlag) {
		return value * attenuation;

	} else if (tuningFlag) {
		return OtAudioUtilities::tuneCV(value, tuningOctaves, tuningSemitones, tuningCents);

	} else {
		return value;
	}
}


//
//	OtCircuitPinClass::getSamples
//

void OtCircuitPinClass::getSamples(float* samples) {
	for (size_t c = 0; c < sourcePin->audioBuffer->getChannelCount(); c++) {
		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			*samples++ = getSample(c, i);
		}
	}
}


//
//	OtCircuitPinClass::setSample
//

void OtCircuitPinClass::setSample(size_t channel, size_t sample, float value) {
	if (attenuationFlag) {
		audioBuffer->set(channel, sample, value * attenuation);

	} else if (tuningFlag) {
		audioBuffer->set(channel, sample, OtAudioUtilities::tuneCV(value, tuningOctaves, tuningSemitones, tuningCents));

	} else {
		audioBuffer->set(channel, sample, value);
	}
}


//
//	OtCircuitPinClass::setSamples
//

void OtCircuitPinClass::setSamples(float* value) {
	for (size_t c = 0; c < audioBuffer->getChannelCount(); c++) {
		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			setSample(c, i, *value++);
		}
	}
}


//
//	OtCircuitPinClass::setSamples
//

void OtCircuitPinClass::setSamples(float value) {
	if (attenuationFlag) {
		audioBuffer->clear(value * attenuation);

	} else if (tuningFlag) {
		audioBuffer->clear(OtAudioUtilities::tuneCV(value, tuningOctaves, tuningSemitones, tuningCents));

	} else {
		audioBuffer->clear(value);
	}
}


//
//	OtCircuitPinClass::serialize
//

nlohmann::json OtCircuitPinClass::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = getTypeName();
	data["id"] = id;
	data["name"] = name;

	if (attenuationFlag) {
		data["attenuation"] = attenuation;
	}

	if (tuningFlag) {
		data["tuningOctaves"] = tuningOctaves;
		data["tuningSemitones"] = tuningSemitones;
		data["tuningCents"] = tuningCents;
	}

	return data;
}


//
//	OtCircuitPinClass::deserialize
//

void OtCircuitPinClass::deserialize(nlohmann::json data, bool restoreIDs, [[maybe_unused]] std::string* basedir) {
	if (restoreIDs) {
		id = data["id"];
	}

	if (attenuationFlag) {
		attenuation = data.value("attenuation", 1.0f);
	}

	if (tuningFlag) {
		tuningOctaves = data.value("tuningOctaves", 0.0f);
		tuningSemitones = data.value("tuningSemitones", 0.0f);
		tuningCents = data.value("tuningCents", 0.0f);
	}
}
