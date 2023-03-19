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

#include "OtGeometryComponent.h"
#include "OtGeometryFactory.h"


//
//	OtGeometryComponent::OtGeometryComponent
//

OtGeometryComponent::OtGeometryComponent() {
	createGeometry("Sphere");
}

OtGeometryComponent::OtGeometryComponent(const std::string& type) {
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
	changed |= ImGui::Checkbox("Transparent", &transparent);
	changed |= ImGui::Checkbox("Cull Back Faces", &cullback);
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
	data["transparent"] = transparent;
	data["cullback"] = cullback;
	data["geometry"] = geometry->serialize();
	return data;
}


//
//	OtGeometryComponent::deserialize
//

void OtGeometryComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	wireframe = data.value("wireframe", false);
	transparent = data.value("transparent", false);
	cullback = data.value("cullback", true);

	if (data.contains("geometry") && data["geometry"].contains("type")) {
		createGeometry(data["geometry"]["type"]);
		geometry->deserialize(data["geometry"]);

	} else {
		createGeometry("Sphere");
	}
}


//
//	OtGeometryComponent::createGeometry
//

void OtGeometryComponent::createGeometry(const std::string& type) {
	auto factory = OtGeometryFactory::instance();

	if (factory->exists(type)) {
		geometry = factory->create(type);
		geometry->computeTangents();

	} else {
		OtExcept("Invalid geometry type [%s]", type.c_str());
	}
}
