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

struct OtGraphLink {
	// constructor
	OtGraphLink(OtGraphPin f, OtGraphPin t, int i=0) : from(f), to(t) {
		id = i ? i : OtGraphGenerateID();
	}

	// (de)serialize
	nlohmann::json serialize();
	void deserialize(nlohmann::json data);

	// properties
	int id;
	OtGraphPin from;
	OtGraphPin to;
};
