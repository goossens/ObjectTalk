//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <memory>

#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtCircuit.h"


//
//	OtCircuitClass::serialize
//

nlohmann::json OtCircuitClass::serialize(std::string* basedir) {
	// serialize circuit data
	auto data = nlohmann::json::object();
	data["id"] = id;
	data["type"] = type;
	data["title"] = title;
	data["x"] = x;
	data["y"] = y;

	// serialize pins
	auto inputs = nlohmann::json::array();
	auto outputs = nlohmann::json::array();

	eachInput([&](OtCircuitPin pin) {
		inputs.push_back(pin->serialize(basedir));
	});

	eachOutput([&](OtCircuitPin pin) {
		outputs.push_back(pin->serialize(basedir));
	});

	data["inputs"] = inputs;
	data["outputs"] = outputs;

	// do any circuit specific serialization
	customSerialize(&data, basedir);

	return data;
}


//
//	OtCircuitClass::deserialize
//

void OtCircuitClass::deserialize(nlohmann::json data, bool restoreIDs, std::string* basedir) {
	// restore ID (if required)
	if (restoreIDs) {
		id = data["id"];
	}

	// restore location and title
	x = data.value("x", 0.0f);
	y = data.value("y", 0.0f);
	title = data.value("title", type);

	// restore input pins
	eachInput([&](OtCircuitPin pin) {
		for (auto& input : data["inputs"]) {
			auto n = input["name"];
			auto t = input["type"];
			auto t1 = pin->getTypeName();

			if (input["name"] == pin->name && input["type"] == pin->getTypeName()) {
				pin->deserialize(input, restoreIDs, basedir);
			}
		}
	});

	// restore output pins
	eachOutput([&](OtCircuitPin pin) {
		for (auto& output : data["outputs"]) {
			if (output["name"] == pin->name && output["type"] == pin->getTypeName()) {
				pin->deserialize(output, restoreIDs, basedir);
			}
		}
	});

	// do any circuit specific deserialization
	customDeserialize(&data, basedir);

	// set flags
	needsSizing = true;
}


//
//	OtCircuitClass::deserializeFromString
//

void OtCircuitClass::deserializeFromString(const std::string& json, bool restoreIDs, std::string* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserialize(data, restoreIDs, basedir);
}


//
//	OtCircuitClass::captureState
//

std::string OtCircuitClass::captureState() {
	return serialize().dump();
}


//
//	OtCircuitClass::captureStateTransaction
//

void OtCircuitClass::captureStateTransaction(const std::string& old) {
	oldState = old;
	newState = serialize().dump();
	needsSaving = true;
}
