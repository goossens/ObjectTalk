//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"

#include "OtGraphPin.h"
#include "OtGraphUtils.h"


//
//	OtGraphLink
//

class OtGraphLinkClass;
using OtGraphLink = std::shared_ptr<OtGraphLinkClass>;

class OtGraphLinkClass {
public:
	// constructor
	inline OtGraphLinkClass(OtGraphPin f, OtGraphPin t, uint32_t i=0) : from(f), to(t) {
		id = i ? i : OtGraphGenerateID();
	}

	// redirect to a new pin
	inline void redirectTo(OtGraphPin newTo) {
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
	nlohmann::json serialize();

	// properties
	uint32_t id;
	OtGraphPin from;
	OtGraphPin to;
};
