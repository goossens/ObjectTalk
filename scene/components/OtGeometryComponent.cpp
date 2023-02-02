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

#include "OtGeometryComponent.h"
#include "OtGeometryFactory.h"


//
//	OtGeometryComponent::OtGeometryComponent
//

OtGeometryComponent::OtGeometryComponent() {
	createGeometry("Box");
}

OtGeometryComponent::OtGeometryComponent(const std::string &type) {
	createGeometry(type);
}


//
//	OtGeometryComponent::renderGUI
//

bool OtGeometryComponent::renderGUI() {
	auto type = geometry->getTypeName();
	bool changed = false;

	if (ImGui::BeginCombo("Type", type)) {
		OtGeometryFactory::instance()->each([&](const char* name) {
			bool isSelectedOne = !std::strcmp(type, name);

			if (ImGui::Selectable(name, isSelectedOne)) {
				if (std::strcmp(type, name)) {
					createGeometry(name);
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

	changed |= ImGui::Checkbox("Wireframe", &wireframe);
	changed |= ImGui::Checkbox("Cull Back", &cullback);
	changed |= geometry->renderGUI();
	return changed;
}


//
//	OtGeometryComponent::serialize
//

nlohmann::json OtGeometryComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["wireframe"] = wireframe;
	data["cullback"] = cullback;
	data["geometry"] = geometry->serialize();
	return data;
}


//
//	OtGeometryComponent::deserialize
//

void OtGeometryComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	wireframe = data["wireframe"];
	cullback = data["cullback"];
	createGeometry(data["geometry"]["type"]);
	geometry->deserialize(data["geometry"]);
}


//
//	OtGeometryComponent::createGeometry
//

void OtGeometryComponent::createGeometry(const std::string& type) {
	auto factory = OtGeometryFactory::instance();

	if (factory->exists(type)) {
		geometry = factory->create(type);

	} else {
		OtExcept("Invalid geometry type [%s]", type.c_str());
	}
}
