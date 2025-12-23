//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
}
