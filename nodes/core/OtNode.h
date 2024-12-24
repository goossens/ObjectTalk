//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtNodesPin.h"
#include "OtNodesUtils.h"


//
//	OtNodeVaryingContext
//

struct OtNodeVaryingContext {
	// contructors
	OtNodeVaryingContext() : index(0), hasVertex(false), hasInstance(false) {}
	OtNodeVaryingContext(int i, const OtVertex& v) : index(i), hasVertex(true), hasInstance(false), vertex(v) {}
	OtNodeVaryingContext(int i, const glm::mat4& in) : index(i), hasVertex(false), hasInstance(true), instance(in) {}

	// properties
	int index;
	bool hasVertex;
	bool hasInstance;
	OtVertex vertex;
	glm::mat4 instance;
};


//
//	OtNode
//

class OtNodeClass;
using OtNode = std::shared_ptr<OtNodeClass>;

class OtNodeClass {
public:
	// node categories
	enum class Category {
		input,
		output,
		field,
		math,
		generate,
		filter,
		transform,
		shape,
		geometry,
		virtualize,
		save,
		probe
	};

	static constexpr const char* categoryNames[] = {
		"Input",
		"Output",
		"Field",
		"Math",
		"Generate",
		"Filter",
		"Transform",
		"Shape",
		"Geometry",
		"Virtualize",
		"Save",
		"Probe"
	};

	// node kind
	enum class Kind {
		fixed,
		varying,
		flexible
	};

	// constructor
	inline OtNodeClass() {
		id = OtNodesGenerateID();
	}

	// destructor
	inline virtual ~OtNodeClass() {}

	// configure node
	virtual void configure() = 0;

	// add pins
	template <typename T>
	inline OtNodesPin addInputPin(const char* name, T& value) {
		OtNodesPin pin = std::make_shared<OtNodesPinImpl<T>>(name, OtNodesPinClass::Direction::input, value);
		pin->node = this;
		inputPins.emplace_back(pin);
		return pin;
	}

	template <typename T>
	inline OtNodesPin addOutputPin(const char* name, T& value) {
		OtNodesPin pin = std::make_shared<OtNodesPinImpl<T>>(name, OtNodesPinClass::Direction::output, value);
		pin->node = this;
		outputPins.emplace_back(pin);
		return pin;
	}

	// (de)serialize
	nlohmann::json serialize(std::string* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);
	void deserializeFromString(const std::string& json, bool restoreIDs=true, std::string* basedir=nullptr);

	virtual inline void customSerialize(nlohmann::json* data, std::string* basedir) {}
	virtual inline void customDeserialize(nlohmann::json* data, std::string* basedir) {}

	// get pin counts
	inline size_t getInputPinCount() { return inputPins.size(); }
	inline size_t getOutputPinCount() { return outputPins.size(); }

	// iterate through all pins
	inline void eachPin(std::function<void(OtNodesPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}

		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// iterate through all input pins
	inline void eachInput(std::function<void(OtNodesPin&)> callback) {
		for (auto& pin : inputPins) {
			callback(pin);
		}
	}

	// iterate through all output pins
	inline void eachOutput(std::function<void(OtNodesPin&)> callback) {
		for (auto& pin : outputPins) {
			callback(pin);
		}
	}

	// do we have a variable output pin?
	inline bool hasVaryingInput() { return varyingInput; }

	// process the varying context (called for each iteration)
	virtual void processVaryingContext(OtNodeVaryingContext& context) {}

	// interfaces
	virtual inline bool onUpdate() { return false; };
	virtual inline void onExecute() {};

	// handle custom section of nodes
	virtual inline void customRendering(float width) {}
	virtual inline float getCustomRenderingWidth() { return 0.0f; }
	virtual inline float getCustomRenderingHeight() { return 0.0f; }

	// special rendering for input nodes
	virtual inline bool customInputRendering(float width) { return false; }

	// public properties
	uint32_t id;
	const char* type;
	Category category;
	Kind kind;
	std::string title;
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	bool selected = false;
	bool needsPlacement = false;
	bool needsSizing = false;
	bool needsEvaluating = false;
	bool needsSaving = false;
	bool permanentMark = false;
	bool temporaryMark = false;

	bool varyingInput = false;

	std::string oldState;
	std::string newState;

protected:
	// private properties
	std::vector<OtNodesPin> inputPins;
	std::vector<OtNodesPin> outputPins;

	void evaluateVariableInputs(OtNodeVaryingContext& context, bool toplevel=true);
};
