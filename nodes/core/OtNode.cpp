//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <memory>

#include "nlohmann/json.hpp"

#include "OtNode.h"


//
//	OtNodeClass::serialize
//

nlohmann::json OtNodeClass::serialize(std::string* basedir) {
	// serialize node data
	auto data = nlohmann::json::object();
	data["id"] = id;
	data["type"] = type;
	data["title"] = title;
	data["x"] = x;
	data["y"] = y;

	// serialize pins
	auto inputs = nlohmann::json::array();
	auto outputs = nlohmann::json::array();

	eachInput([&](OtNodesPin& pin) {
		inputs.push_back(pin->serialize(basedir));
	});

	eachOutput([&](OtNodesPin& pin) {
		outputs.push_back(pin->serialize(basedir));
	});

	data["inputs"] = inputs;
	data["outputs"] = outputs;

	// do any node specific serialization
	customSerialize(&data, basedir);

	return data;
}


//
//	OtNodeClass::deserialize
//

void OtNodeClass::deserialize(nlohmann::json data, bool restoreIDs, std::string* basedir) {
	// restore ID (if required)
	if (restoreIDs) {
		id = data["id"];
	}

	// restore location and title
	x = data.value("x", 0.0f);
	y = data.value("y", 0.0f);
	title = data.value("title", type);

	// restore input pins
	eachInput([&](OtNodesPin& pin) {
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
	eachOutput([&](OtNodesPin& pin) {
		for (auto& output : data["outputs"]) {
			if (output["name"] == pin->name && output["type"] == pin->getTypeName()) {
				pin->deserialize(output, restoreIDs, basedir);
			}
		}
	});

	// do any node specific deserialization
	customDeserialize(&data, basedir);

	// set flags
	needsEvaluating = true;
	needsSizing = true;
}


//
//	OtNodeClass::deserializeFromString
//

void OtNodeClass::deserializeFromString(const std::string& json, bool restoreIDs, std::string* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserialize(data, restoreIDs, basedir);
}


//
//	OtNodeClass::evaluateVariableInputs
//

bool OtNodeClass::evaluateVariableInputs(bool toplevel) {
	bool changed = kind == varying;

	// process all input pins
	eachInput([&](OtNodesPin& pin){
		// only look at connected pins that are marked as varying
		if (pin->isConnected() && pin->isVarying()) {
			// see if source node made changes
			if (pin->sourcePin->node->evaluateVariableInputs(false)) {
				pin->evaluate();

				if (pin->needsEvaluating) {
					pin->needsEvaluating = false;
					changed = true;
				}
			}
		}
	});

	if (changed && !toplevel) {
		onExecute();
	}

	return changed;
}
