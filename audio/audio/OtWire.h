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

#include "nlohmann/json_fwd.hpp"

#include "OtCircuitPin.h"
#include "OtCircuitUtils.h"


//
//	OtWire
//

class OtWireClass;
using OtWire = std::shared_ptr<OtWireClass>;

class OtWireClass {
public:
	// constructor
	inline OtWireClass(OtCircuitPin f, OtCircuitPin t, uint32_t i=0) : from(f), to(t) {
		id = i ? i : OtCircuitGenerateID();
	}

	// redirect to a new pin
	inline void redirectTo(OtCircuitPin newTo) {
		to = newTo;
	}

	// (dis)connect the pins
	inline void connect() {
		to->connectToSource(from);
		from->connectToDestination();
	}

	inline void disconnect() {
		to->disconnectFromSource();
		from->disconnectFromDestination();
	}

	// serialize
	nlohmann::json serialize(std::string* basedir=nullptr);

	// properties
	uint32_t id;
	OtCircuitPin from;
	OtCircuitPin to;
};
