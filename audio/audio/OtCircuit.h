//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtVertex.h"

#include "OtCircuitPin.h"
#include "OtCircuitUtils.h"


//
//	OtCircuit
//

class OtCircuitClass;
using OtCircuit = std::shared_ptr<OtCircuitClass>;

class OtCircuitClass {
public:
	// circuit categories
	enum class Category {
		input,
		output,
		generator,
		effect,
		probe
	};

	static constexpr const char* categoryNames[] = {
		"Input",
		"Output",
		"Generator",
		"Effect",
		"Probe"
	};

	// constructor
	inline OtCircuitClass() {
		id = OtCircuitGenerateID();
	}

	// destructor
	inline virtual ~OtCircuitClass() {}

	// configure circuit
	virtual void configure() = 0;

	// add pins
	inline OtCircuitPin addInputPin(const char* name, OtCircuitPinClass::Type type) {
		OtCircuitPin pin = std::make_shared<OtCircuitPinClass>(name, type, OtCircuitPinClass::Direction::input);
		pin->circuit = this;
		inputPins.emplace_back(pin);
		return pin;
	}

	inline OtCircuitPin addOutputPin(const char* name, OtCircuitPinClass::Type type) {
		OtCircuitPin pin = std::make_shared<OtCircuitPinClass>(name, type, OtCircuitPinClass::Direction::output);
		pin->circuit = this;
		outputPins.emplace_back(pin);
		return pin;
	}

	// (de)serialize
	nlohmann::json serialize(std::string* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);
	void deserializeFromString(const std::string& json, bool restoreIDs=true, std::string* basedir=nullptr);

	virtual inline void customSerialize([[maybe_unused]] nlohmann::json* data, [[maybe_unused]] std::string* basedir) {}
	virtual inline void customDeserialize([[maybe_unused]] nlohmann::json* data, [[maybe_unused]] std::string* basedir) {}

	// get pin counts
	inline size_t getInputPinCount() { return inputPins.size(); }
	inline size_t getOutputPinCount() { return outputPins.size(); }

	// iterate through all pins
	inline void eachPin(std::function<void(OtCircuitPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}

		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// iterate through all input pins
	inline void eachInput(std::function<void(OtCircuitPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}
	}

	// iterate through all output pins
	inline void eachOutput(std::function<void(OtCircuitPin&)> callback) {
		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// interfaces
	virtual inline void execute([[maybe_unused]] size_t sampleRate, [[maybe_unused]] size_t samples) {};

	// handle custom section of circuits
	virtual inline void customRendering([[maybe_unused]] float itemWidth) {}
	virtual inline float getCustomRenderingWidth() { return 0.0f; }
	virtual inline float getCustomRenderingHeight() { return 0.0f; }

	// special rendering for input circuits
	virtual inline bool customInputRendering([[maybe_unused]] float itemWidth) { return false; }

	// public properties
	uint32_t id;
	const char* type;
	Category category;
	std::string title;
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	bool selected = false;
	bool needsPlacement = false;
	bool needsSizing = false;
	bool needsSaving = false;
	bool permanentMark = false;
	bool temporaryMark = false;

	std::string oldState;
	std::string newState;

	std::string error;

protected:
	// private properties
	std::vector<OtCircuitPin> inputPins;
	std::vector<OtCircuitPin> outputPins;
};
