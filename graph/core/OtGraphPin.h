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
#include <functional>
#include <memory>
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtGraphDataTypes.h"
#include "OtGraphUtils.h"


//
//	Forward definitions
//

class OtGraphNodeClass;


//
//	OtGraphPin
//

class OtGraphPinClass;
using OtGraphPin = std::shared_ptr<OtGraphPinClass>;
typedef std::function<void()> OtGraphPinRenderer;

class OtGraphPinClass {
public:
	enum {
		inputPin,
		outputPin
	};

	// constructor
	inline OtGraphPinClass(const char* n, int d) : name(n), direction(d) {
		id = OtGraphGenerateID();
	}

	// destructor
	virtual inline ~OtGraphPinClass() {};

	// handle custom renderer
	inline void addRenderer(OtGraphPinRenderer renderer, float width) {
		render = renderer;
		renderingWidth = width;
		hasRenderer = true;
	}

	// check direction
	inline bool isInput() { return direction == inputPin; }
	inline bool isOutput() { return direction == outputPin; }

	// handle connections
	virtual void connect(OtGraphPin sourcePin) = 0;
	virtual void disconnect() = 0;
	inline OtGraphPin getSource() { return sourcePin; }
	inline bool isConnected() { return sourcePin != nullptr; }

	// evaluate the pin
	virtual void evaluate() = 0;

	// (de)serialize
	nlohmann::json serialize(std::filesystem::path* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::filesystem::path* basedir=nullptr);

	// get the pin type as a string
	inline std::string getTypeName() { return OtGraphDataTypeNames[type]; }

	// properties
	int type;
	uint32_t id;
	const char* name;
	int direction;
	OtGraphNodeClass* node;

	OtGraphPinRenderer render = [](){};
	float renderingWidth{0.0f};
	bool hasRenderer{false};

	OtGraphPin sourcePin;
};


//
//	OtGraphPinImpl
//

template <typename T>
class OtGraphPinImpl : public OtGraphPinClass {
public:
	// constructor
	inline OtGraphPinImpl(const char* n, int d, T* v) : OtGraphPinClass(n, d) {
		static_assert(OtTypeListIndexOf<T, OtGraphDataTypes>() != -1, "Data type not allowed for graph node pin");
		type = OtTypeListIndexOf<T, OtGraphDataTypes>();
		value = v;
		defaultValue = *value;
	}

	// handle connections
	inline void connect(OtGraphPin srcPin) override {
		sourcePin = srcPin;
		source = srcPin ? std::dynamic_pointer_cast<OtGraphPinImpl<T>>(srcPin)->value : nullptr;
	}

	inline void disconnect() override {
		*value = defaultValue;
		sourcePin = nullptr;
		source = nullptr;
	}

	inline bool isConnected() { return sourcePin != nullptr; }

	// evaluate the pin
	void evaluate() override {
		if (source) {
			*value = *source;
		}
	}

	// properties
	T* value;
	T defaultValue;
	T* source = nullptr;
};

