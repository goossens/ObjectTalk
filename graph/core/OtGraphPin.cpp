//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGraphPin.h"


//
//	Constants
//

static constexpr const char* pinTypeNames[] = { "bool", "float" };


//
//	OtGraphPinClass::serialize
//

nlohmann::json OtGraphPinClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = getTypeName();
	data["id"] = id;
	data["name"] = name;

	if (type == OtTypeListIndexOf<bool, OtGraphPinTypes>()) {
		data["value"] = *dynamic_cast<OtGraphPinImpl<bool>*>(this)->value;

	} else if (type == OtTypeListIndexOf<float, OtGraphPinTypes>()) {
		data["value"] = *dynamic_cast<OtGraphPinImpl<float>*>(this)->value;
	}

	return data;
}


//
//	OtGraphPinClass::deserialize
//

void OtGraphPinClass::deserialize(nlohmann::json data) {
	id = data["id"];

	if (type == OtTypeListIndexOf<bool, OtGraphPinTypes>()) {
		*dynamic_cast<OtGraphPinImpl<bool>*>(this)->value = data["value"];

	} else if (type == OtTypeListIndexOf<float, OtGraphPinTypes>()) {
		*dynamic_cast<OtGraphPinImpl<float>*>(this)->value = data["value"];
	}
}
