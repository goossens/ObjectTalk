//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <random>

#include "nlohmann/json.hpp"

#include "OtCoreComponent.h"


//
//	OtCoreComponent::OtCoreComponent
//

OtCoreComponent::OtCoreComponent() {
	assignNewUuid();
	tag = "untitled";
}

OtCoreComponent::OtCoreComponent(uint32_t u, const std::string& t) {
	if (u) {
		uuid = u;

	} else {
		assignNewUuid();
	}

	tag = t;
}


//
//	OtCoreComponent::assignNewUuid
//

void OtCoreComponent::assignNewUuid() {
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint32_t> distribution;
	uuid = distribution(engine);
}


//
//	OtCoreComponent::serialize
//

nlohmann::json OtCoreComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["uuid"] = uuid;
	data["tag"] = tag;
	return data;
}


//
//	OtCoreComponent::deserialize
//

void OtCoreComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	uuid = data["uuid"];
	tag = data["tag"];
}
