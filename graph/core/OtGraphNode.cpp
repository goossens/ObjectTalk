//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <memory>

#include "nlohmann/json.hpp"

#include "OtGraphNode.h"


//
//	OtGraphNodeClass::serialize
//

nlohmann::json OtGraphNodeClass::serialize(std::filesystem::path* basedir) {
	// serialize node data
	auto data = nlohmann::json::object();
	data["name"] = name;
	data["id"] = id;
	data["x"] = x;
	data["y"] = y;

	// serialize pins
	auto inputs = nlohmann::json::array();
	auto outputs = nlohmann::json::array();

	eachInput([&] (OtGraphPin& pin) {
		inputs.push_back(pin->serialize(basedir));
	});

	eachOutput([&] (OtGraphPin& pin) {
		outputs.push_back(pin->serialize(basedir));
	});

	data["inputs"] = inputs;
	data["outputs"] = outputs;

	// do any node specific serialization
	customSerialize(&data, basedir);

	return data;
}


//
//	OtGraphNodeClass::deserialize
//

void OtGraphNodeClass::deserialize(nlohmann::json data, bool restoreIDs, std::filesystem::path* basedir) {
	// restore ID (if required)
	if (restoreIDs) {
		id = data["id"];
	}

	// restore location
	x = data.value("x", 0.0f);
	y = data.value("y", 0.0f);

	// restore input pins
	eachInput([&] (OtGraphPin& pin) {
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
	eachOutput([&] (OtGraphPin& pin) {
		for (auto& output : data["outputs"]) {
			if (output["name"] == pin->name && output["type"] == pin->getTypeName()) {
				pin->deserialize(output, restoreIDs, basedir);
			}
		}
	});

	// do any node specific deserialization
	customDeserialize(&data, basedir);
}


//
//	OtGraphNodeClass::deserializeFromString
//

void OtGraphNodeClass::deserializeFromString(const std::string& json, bool restoreIDs, std::filesystem::path* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserialize(data, restoreIDs, basedir);
}
