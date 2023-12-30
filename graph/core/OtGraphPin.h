//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <memory>
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtTypeList.h"

#include "OtGraphUtils.h"


//
//	Forward definitions
//

class OtGraphNodeClass;
using OtGraphNode = std::shared_ptr<OtGraphNodeClass>;


//
//	Pin types
//

using OtGraphPinTypes = OtTypeList<bool, float>;
static constexpr const char* OtGraphPinTypeNames[] = { "bool", "float" };


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

	// (de)serialize
	nlohmann::json serialize();
	void deserialize(nlohmann::json data, bool restoreIDs=true);

	// get the pin type as a string
	inline std::string getTypeName() { return OtGraphPinTypeNames[type]; }

	// see if pin content changed
	virtual bool onCheck() = 0;

	// properties
	int type;
	uint32_t id;
	const char* name;
	int direction;
	OtGraphPinRenderer render = [](){};
	float renderingWidth{0.0f};
	bool hasRenderer{false};

	OtGraphNode node; // set by addInputPin or addOutputPin in OtGraphNodeClass
	OtGraphPin sourcePin;
	bool updated;
};


//
//	OtGraphPinImpl
//

template <typename T>
class OtGraphPinImpl : public OtGraphPinClass {
public:
	// constructor
	inline OtGraphPinImpl(const char* n, int d, T* v) : OtGraphPinClass(n, d) {
		static_assert(OtTypeListIndexOf<T, OtGraphPinTypes>() != -1, "Data type not allowed for graph node pin");
		type = OtTypeListIndexOf<T, OtGraphPinTypes>();
		value = v;
	}

	// handle connections
	inline void connect(OtGraphPin srcPin) override {
		sourcePin = srcPin;
		source = srcPin ? std::dynamic_pointer_cast<OtGraphPinImpl<T>>(srcPin)->value : nullptr;
	}

	inline void disconnect() override {
		sourcePin = nullptr;
		source = nullptr;
	}

	inline bool isConnected() { return sourcePin != nullptr; }

	// update pin value from source (if avalable)
	inline bool onCheck() override {
		bool changed;

		if (source) {
			changed = *value != *source;
			*value = *source;

		} else {
			changed = updated;
			updated = false;
		}

		return changed;
	}

	// properties
	T* value;
	T* source = nullptr;
};

