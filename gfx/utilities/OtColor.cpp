//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtColor.h"


//
//	to_json
//

void to_json(nlohmann::json& j, const OtColor& c) {
	j = nlohmann::json{c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()};
}


//
//	from_json
//

void from_json(const nlohmann::json& j, OtColor& c) {
	c.set(j[0], j[1], j[2], j[3]);
}
