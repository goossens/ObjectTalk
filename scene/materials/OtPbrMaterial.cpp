//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtReal.h"

#include "OtGlm.h"
#include "OtVec3.h"
#include "OtVec4.h"

#include "OtPbrMaterial.h"
#include "OtPathTools.h"


//
//	OtPbrMaterialClass::renderGUI
//

#define W() ImGui::SetNextItemWidth(250.0f)

bool OtPbrMaterialClass::renderGUI() {
	bool changed = false;
	float size = ImGui::GetFrameHeight();

	// render button
	if (ImGui::Button("e", ImVec2(size, size))) {
		ImGui::OpenPopup("PbrPopup");
	}

	// open popup (if required)
	if (ImGui::BeginPopup("PbrPopup")) {
		if (ImGui::BeginTable("layout", 3)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Fixed Values");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texure Maps");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit4("##albedoColor", glm::value_ptr(albedo));
			ImGui::TableNextColumn(); W(); changed |= albedoTexture.renderGUI("##albedoTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Albedo");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); W(); changed |= normalTexture.renderGUI("##normalTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Normals");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::SliderFloat("##metallic", &metallic, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); W(); changed |= metallicRoughnessTexture.renderGUI("##metallicRoughnessTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Metallic");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); ImGui::SliderFloat("##roughness", &roughness, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Roughness");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##emissive", glm::value_ptr(emissive));
			ImGui::TableNextColumn(); W(); changed |= emissiveTexture.renderGUI("##emissiveTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Emissive");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::SliderFloat("##ambientOcclusion", &ao, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); W(); changed |= aoTexture.renderGUI("##aoTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Ambient Occlusion");
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtPbrMaterialClass::serialize
//

nlohmann::json OtPbrMaterialClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data.update(OtMaterialClass::serialize(basedir));

	data["albedo"] = albedo;
	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["emissive"] = emissive;
	data["ao"] = ao;

	data["albedoTexture"] = OtPathGetRelative(albedoTexture.getPath(), basedir);
	data["normalTexture"] = OtPathGetRelative(normalTexture.getPath(), basedir);
	data["metallicRoughnessTexture"] = OtPathGetRelative(metallicRoughnessTexture.getPath(), basedir);
	data["emissiveTexture"] = OtPathGetRelative(emissiveTexture.getPath(), basedir);
	data["aoTexture"] = OtPathGetRelative(aoTexture.getPath(), basedir);
	return data;
}


//
//	OtPbrMaterialClass::deserialize
//

void OtPbrMaterialClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	albedo = data.value("albedo", glm::vec4(1.0f));
	metallic = data.value("metallic", 0.5f);
	roughness = data.value("roughness", 0.5f);
	emissive = data.value("emissive", glm::vec3(0.0f));
	ao = data.value("ao", 1.0f);

	albedoTexture = OtPathGetAbsolute(data, "albedoTexture", basedir);
	normalTexture = OtPathGetAbsolute(data, "normalTexture", basedir);
	metallicRoughnessTexture = OtPathGetAbsolute(data, "metallicRoughnessTexture", basedir);
	emissiveTexture = OtPathGetAbsolute(data, "emissiveTexture", basedir);
	aoTexture = OtPathGetAbsolute(data, "aoTexture", basedir);
}


//
//	OtPbrMaterialClass::setAlbedo
//

void OtPbrMaterialClass::setAlbedo(OtObject object) {
	if (object.isKindOf<OtVec4Class>()) {
		albedo = (glm::vec4) OtVec4(object);

	} else if (object.isKindOf<OtRealClass>()) {
		albedo = glm::vec4(object->operator float());

	} else {
		OtError("Expected a [Vec4] or a [Real], not a [%s]", object->getType()->getName().c_str());
	}
}


//
//	OtPbrMaterialClass::setEmissive
//

void OtPbrMaterialClass::setEmissive(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		emissive = (glm::vec3) OtVec3(object);

	} else if (object.isKindOf<OtRealClass>()) {
		emissive = glm::vec3(object->operator float());

	} else {
		OtError("Expected a [Vec3] or a [Real], not a [%s]", object->getType()->getName().c_str());
	}
}


//
//	OtPbrMaterialClass::getMeta
//

OtType OtPbrMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPbrMaterialClass>("PbrMaterial", OtMaterialClass::getMeta());
	}

	return type;
}
