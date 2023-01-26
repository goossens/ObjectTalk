//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <random>

#include "nlohmann/json.hpp"

#include "OtUuidComponent.h"


//
//	OtUuidComponent::OtUuidComponent
//

OtUuidComponent::OtUuidComponent() {
	assignNewUuid();
}


//
//	OtUuidComponent::assignNewUuid
//

void OtUuidComponent::assignNewUuid() {
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint32_t> distribution;
	uuid = distribution(engine);
}


//
//	OtUuidComponent::serialize
//

nlohmann::json OtUuidComponent::serialize() {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["uuid"] = uuid;
	return data;
}


//
//	OtUuidComponent::deserialize
//

void OtUuidComponent::deserialize(nlohmann::json data) {
	uuid = data["uuid"];
}
