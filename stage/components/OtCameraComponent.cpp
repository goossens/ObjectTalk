//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtCameraComponent.h"
#include "OtCameraFactory.h"


//
//	OtCameraComponent::OtCameraComponent
//

OtCameraComponent::OtCameraComponent() {
	createCamera("Perspective");
}

OtCameraComponent::OtCameraComponent(const std::string &type) {
	createCamera(type);
}


//
//	OtCameraComponent::renderGUI
//

bool OtCameraComponent::renderGUI() {
	auto type = camera->getTypeName();
	bool changed = false;

	if (ImGui::BeginCombo("Type", type)) {
		OtCameraFactory::instance()->each([&](const char* name) {
			bool isSelectedOne = !std::strcmp(type, name);

			if (ImGui::Selectable(name, isSelectedOne)) {
				if (std::strcmp(type, name)) {
					createCamera(name);
				}
			}

			// ensure selected entry is in focus
			if (isSelectedOne) {
				ImGui::SetItemDefaultFocus();
			}
		});

		ImGui::EndCombo();
	}

	changed |= camera->renderGUI();
	return changed;
}


//
//	OtCameraComponent::serialize
//

nlohmann::json OtCameraComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["camera"] = camera->serialize();
	return data;
}


//
//	OtCameraComponent::deserialize
//

void OtCameraComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	createCamera(data["camera"]["type"]);
	camera->deserialize(data["camera"]);
}


//
//	OtCameraComponent::createCamera
//

void OtCameraComponent::createCamera(const std::string& type) {
	auto factory = OtCameraFactory::instance();

	if (factory->exists(type)) {
		camera = factory->create(type);

	} else {
		OtExcept("Invalid camera type [%s]", type.c_str());
	}
}


