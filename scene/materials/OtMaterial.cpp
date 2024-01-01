//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtMaterial.h"


//
//	OtMaterialClass::serialize
//

nlohmann::json OtMaterialClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	return data;
}


//
//	OtMaterialClass::deserialize
//

void OtMaterialClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
}


//
//	OtMaterialClass::getMeta
//

OtType OtMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtMaterialClass>("Material", OtObjectClass::getMeta());
	}

	return type;
}
