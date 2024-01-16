//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtGraphPin.h"
#include "OtGraphUtils.h"


//
//	OtGraphNode
//

class OtGraphNodeClass;
using OtGraphNode = std::shared_ptr<OtGraphNodeClass>;

class OtGraphNodeClass {
public:
	// node categories
	enum {
		input,
		output,
		math,
		generator
	};

	// constructor
	inline OtGraphNodeClass(const char* t, int c) : type(t), title(t), category(c) {
		id = OtGraphGenerateID();
	}

	// destructor
	inline virtual ~OtGraphNodeClass() {}

	// configure node
	virtual void configure() = 0;

	// add pins
	template <typename T>
	inline OtGraphPin addInputPin(const char* name, T& value) {
		OtGraphPin pin = std::make_shared<OtGraphPinImpl<T>>(name, OtGraphPinClass::inputPin, &value);
		pin->node = this;
		inputPins.emplace_back(pin);
		return pin;
	}

	template <typename T>
	inline OtGraphPin addOutputPin(const char* name, T& value) {
		OtGraphPin pin = std::make_shared<OtGraphPinImpl<T>>(name, OtGraphPinClass::outputPin, &value);
		pin->node = this;
		outputPins.emplace_back(pin);
		return pin;
	}

	// (de)serialize
	nlohmann::json serialize(std::filesystem::path* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::filesystem::path* basedir=nullptr);
	void deserializeFromString(const std::string& json, bool restoreIDs=true, std::filesystem::path* basedir=nullptr);

	// get pin counts
	inline size_t getInputPinCount() { return inputPins.size(); }
	inline size_t getOutputPinCount() { return outputPins.size(); }

	// iterate through all pins
	inline void eachPin(std::function<void(OtGraphPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}

		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// iterate through all input pins
	inline void eachInput(std::function<void(OtGraphPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}
	}

	// iterate through all output pins
	inline void eachOutput(std::function<void(OtGraphPin&)> callback) {
		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// interfaces
	virtual inline void onUpdate() {};
	virtual inline void onStart() {};
	virtual inline void onExecute() {};
	virtual inline void onEnd() {};

	// handle custom section of notes
	virtual inline void customRendering() {}
	virtual inline float getCustomRenderingWidth() { return 0.0f; }
	virtual inline float getCustomRenderingHeight() { return 0.0f; }

	// public properties
	uint32_t id;
	const char* type;
	int category;
	std::string title;
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	bool selected = false;
	bool needsPlacement = false;
	bool needsEvaluating = false;
	bool needsSaving = false;
	bool permanentMark = false;
	bool temporaryMark = false;

	std::string oldState;
	std::string newState;

protected:
	// private properties
	std::vector<OtGraphPin> inputPins;
	std::vector<OtGraphPin> outputPins;

	virtual inline void customSerialize(nlohmann::json* data, std::filesystem::path* basedir) {}
	virtual inline void customDeserialize(nlohmann::json* data, std::filesystem::path* basedir) {}
};
