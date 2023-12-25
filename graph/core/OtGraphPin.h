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

class OtGraphPinClass {
public:
	enum {
		inputPin,
		outputPin
	};

	// constructor
	OtGraphPinClass(const char* n, int d) : name(n), direction(d) {
		id = OtGraphGenerateID();
	}

	// destructor
	virtual ~OtGraphPinClass() {};

	// check direction
	bool isInput() { return direction == inputPin; }
	bool isOutput() { return direction == outputPin; }

	// specify a source pin
	virtual void setSource(OtGraphPin sourcePin) = 0;
	virtual void unsetSource() = 0;

	// (de)serialize
	nlohmann::json serialize();
	void deserialize(nlohmann::json data);

	// get the pin type as a string
	std::string getTypeName() { return OtGraphPinTypeNames[type]; }

	// see if pin content changed
	virtual bool onCheck() = 0;

	// properties
	int type;
	uint32_t id;
	const char* name;
	int direction;

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
	OtGraphPinImpl(const char* n, int d, T* v) : OtGraphPinClass(n, d) {
		static_assert(OtTypeListIndexOf<T, OtGraphPinTypes>() != -1, "Data type not allowed for graph node pin");
		type = OtTypeListIndexOf<T, OtGraphPinTypes>();
		value = v;
	}

	// specify the source pin
	void setSource(OtGraphPin srcPin) override {
		sourcePin = srcPin;
		source = srcPin ? std::dynamic_pointer_cast<OtGraphPinImpl<T>>(srcPin)->value : nullptr;
	}

	void unsetSource() override {
		sourcePin = nullptr;
		source = nullptr;
	}

	// update pin value from source (if avalable)
	bool onCheck() override {
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

	// set new value
	void setValue(T v) {
		*value = v;
		updated = true;
	}

	// properties
	T* value;
	T* source = nullptr;
};

