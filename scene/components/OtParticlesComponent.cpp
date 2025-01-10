//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtPath.h"
#include "OtUi.h"

#include "OtParticlesComponent.h"


//
//	OtParticlesComponent::OtParticlesComponent
//

OtParticlesComponent::OtParticlesComponent() {
	particles = std::make_shared<OtParticles>();
}


//
//	OtParticlesComponent::renderUI
//

bool OtParticlesComponent::renderUI() {
	return settings.renderUI();
}


//
//	OtParticlesComponent::serialize
//

nlohmann::json OtParticlesComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["settings"] = settings.serialize(basedir);
	return data;
}


//
//	OtParticlesComponent::deserialize
//

void OtParticlesComponent::deserialize(nlohmann::json data, std::string* basedir) {
	settings.deserialize(data["settings"], basedir);
}
