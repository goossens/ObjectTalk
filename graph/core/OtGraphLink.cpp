//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGraphLink.h"


//
//	OtGraphLinkClass::serialize
//

nlohmann::json OtGraphLinkClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["id"] = id;
	data["from"] = from->id;
	data["to"] = to->id;
	return data;
}
