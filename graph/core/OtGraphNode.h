//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "nlohmann/json_fwd.hpp"

#include "OtGraphPin.h"
#include "OtGraphUtils.h"


//
//	OtGraphNode
//

class OtGraphNodeClass;
using OtGraphNode = std::shared_ptr<OtGraphNodeClass>;

class OtGraphNodeClass : public std::enable_shared_from_this<OtGraphNodeClass> {
public:
	// constructor
	OtGraphNodeClass(const char* n) : name(n) {
		id = OtGraphGenerateID();
	}

	// destructor
	virtual ~OtGraphNodeClass() {}

	// configure node
	virtual void configure() = 0;

	// add pins
	template <typename T>
	void addInputPin(const char* name, T& value) {
		OtGraphPin pin = std::make_shared<OtGraphPinImpl<T>>(name, OtGraphPinClass::inputPin, &value);
		pin->node = shared_from_this();
		inputPins.emplace_back(pin);
	}

	template <typename T>
	void addOutputPin(const char* name, T& value) {
		OtGraphPin pin = std::make_shared<OtGraphPinImpl<T>>(name, OtGraphPinClass::outputPin, &value);
		pin->node = shared_from_this();
		outputPins.emplace_back(pin);
	}

	// (de)serialize
	nlohmann::json serialize();
	void deserialize(nlohmann::json data, bool restoreID=true);

	// iterate through all input pins
	void eachInput(std::function<void(OtGraphPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}
	}

	// iterate through all output pins
	void eachOutput(std::function<void(OtGraphPin&)> callback) {
		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// interfaces
	virtual bool onCheck();
	virtual void onStart() {};
	virtual bool onExecute() { return false; };
	virtual void onEnd() {};

	// public properties
	uint32_t id;
	const char* name;
	float x;
	float y;
	bool needsPlacement;

private:
	// private properties
	std::vector<OtGraphPin> inputPins;
	std::vector<OtGraphPin> outputPins;
};
