//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtGlm.h"
#include "OtUi.h"

#include "OtPathTools.h"
#include "OtMaterialComponent.h"
#include "OtMaterialFactory.h"


//
//	OtMaterialComponent::OtMaterialComponent
//

OtMaterialComponent::OtMaterialComponent() {
	createMaterial("PBR");
}

OtMaterialComponent::OtMaterialComponent(const std::string& type) {
	createMaterial(type);
}


//
//	OtMaterialComponent::renderUI
//

bool OtMaterialComponent::renderUI() {
	auto type = material->getTypeName();
	bool changed = false;

	auto itemWidth = ImGui::CalcItemWidth();
	auto itemHeight = ImGui::GetFrameHeight();
	float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
	ImGui::SetNextItemWidth(itemWidth - itemHeight - spacing);

	if (ImGui::BeginCombo("##Type", type)) {
		OtMaterialFactory::instance()->each([&](const char* name) {
			bool isSelectedOne = !std::strcmp(type, name);

			if (ImGui::Selectable(name, isSelectedOne)) {
				if (std::strcmp(type, name)) {
					createMaterial(name);
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

	ImGui::SameLine(0.0f, spacing);
	changed |= material->renderUI();

	ImGui::SameLine(0.0f, spacing);
	ImGui::TextUnformatted("Type");
	return changed;
}


//
//	OtMaterialComponent::serialize
//

nlohmann::json OtMaterialComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["material"] = material->serialize(basedir);
	return data;
}


//
//	OtMaterialComponent::deserialize
//

void OtMaterialComponent::deserialize(nlohmann::json data, std::string* basedir) {
	if (data.contains("material") && data["material"].contains("type")) {
		createMaterial(data["material"]["type"]);
		material->deserialize(data["material"], basedir);

	} else {
		createMaterial("PBR");
	}
}


//
//	OtMaterialComponent::createMaterial
//

void OtMaterialComponent::createMaterial(const std::string& type) {
	auto factory = OtMaterialFactory::instance();

	if (factory->exists(type)) {
		material = factory->create(type);

	} else {
		OtError("Internal error: invalid material type [%s]", type.c_str());
	}
}
