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

nlohmann::json OtGraphNodeClass::serialize() {
	auto data = nlohmann::json::object();
	data["name"] = name;
	data["id"] = id;
	data["x"] = x;
	data["y"] = y;

	auto inputs = nlohmann::json::array();
	auto outputs = nlohmann::json::array();

	eachInput([&] (OtGraphPin& pin) {
		inputs.push_back(pin->serialize());
	});

	eachOutput([&] (OtGraphPin& pin) {
		outputs.push_back(pin->serialize());
	});

	data["inputs"] = inputs;
	data["outputs"] = outputs;
	return data;
}


//
//	OtGraphNodeClass::deserialize
//

void OtGraphNodeClass::deserialize(nlohmann::json data, bool restoreID) {
	// restore ID (if required)
	if (restoreID) {
		id = data["id"];
	}

	// restore location
	x = data.value("x", 0.0f);
	y = data.value("y", 0.0f);

	// restore input pins
	eachInput([&] (OtGraphPin& pin) {
		for (auto& input : data["inputs"]) {
			if (input["name"] == pin->name && input["type"] == pin->getTypeName()) {
				pin->deserialize(input);
			}
		}
	});

	// restore output pins
	eachOutput([&] (OtGraphPin& pin) {
		for (auto& output : data["output"]) {
			if (output["name"] == pin->name && output["type"] == pin->getTypeName()) {
				pin->deserialize(output);
			}
		}
	});
}


//
//	OtGraphNodeClass::onCheck
//

bool OtGraphNodeClass::onCheck()
{
	bool changed = false;

	eachInput([&changed](OtGraphPin& pin) {
		changed |= pin->onCheck();
	});

	return changed;
}
