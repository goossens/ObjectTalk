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
	updateCamera();
}

OtCameraComponent::OtCameraComponent(const std::string &t) {
	type = t;
	updateCamera();
}


//
//	OtCameraComponent::updateCamera
//

void OtCameraComponent::updateCamera() {
	auto factory = OtCameraFactory::instance();

	if (factory->exists(type)) {
		camera = factory->create(type);

	} else {
		OtExcept("Invalid camera type [%s]", type.c_str());
	}
}


//
//	OtCameraComponent::renderGUI
//

bool OtCameraComponent::renderGUI() {
	bool changed = false;

	if (ImGui::BeginCombo("Type", type.c_str())) {
		OtCameraFactory::instance()->each([&](const char* name) {
			bool isSelectedOne = type == name;

			if (ImGui::Selectable(name, isSelectedOne)) {
				if (type != name) {
					type = name;
					changed = true;
				}
			}

			// ensure selected entry is in focus
			if (isSelectedOne) {
				ImGui::SetItemDefaultFocus();
			}
		});

		ImGui::EndCombo();
	}

	if (changed) {
		updateCamera();
	}

	changed |= camera->renderGUI();
	return changed;
}


//
//	OtCameraComponent::serialize
//

nlohmann::json OtCameraComponent::serialize() {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["type"] = type;
	return data;
}


//
//	OtCameraComponent::deserialize
//

void OtCameraComponent::deserialize(nlohmann::json data) {
	type = data["type"];
	updateCamera();
}
