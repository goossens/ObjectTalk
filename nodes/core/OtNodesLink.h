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

#include "nlohmann/json_fwd.hpp"

#include "OtNodesPin.h"
#include "OtNodesUtils.h"


//
//	OtNodesLink
//

class OtNodesLinkClass;
using OtNodesLink = std::shared_ptr<OtNodesLinkClass>;

class OtNodesLinkClass {
public:
	// constructor
	inline OtNodesLinkClass(OtNodesPin f, OtNodesPin t, uint32_t i=0) : from(f), to(t) {
		id = i ? i : OtNodesGenerateID();
	}

	// redirect to a new pin
	inline void redirectTo(OtNodesPin newTo) {
		to = newTo;
	}

	// (dis)connect the pins
	inline void connect() {
		to->connect(from);
	}

	inline void disconnect() {
		to->disconnect();
	}

	// serialize
	nlohmann::json serialize(std::string* basedir=nullptr);

	// properties
	uint32_t id;
	OtNodesPin from;
	OtNodesPin to;
};
