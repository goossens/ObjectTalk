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

#include "OtBlendMapMaterial.h"
#include "OtPathTools.h"


//
//	OtBlendMapMaterialClass::renderGUI
//

#define W() ImGui::SetNextItemWidth(250.0f)

bool OtBlendMapMaterialClass::renderGUI() {
	bool changed = false;
	float size = ImGui::GetFrameHeight();

	// render button
	if (ImGui::Button("e", ImVec2(size, size))) {
		ImGui::OpenPopup("BlendMapPopup");
	}

	// open popup (if required)
	if (ImGui::BeginPopup("BlendMapPopup")) {
		if (ImGui::BeginTable("layout", 3)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Normals");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= blendMapTexture.renderGUI("##blendMapTexture");
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("BlendMap");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= noneTexture.renderGUI("##noneTexture");
			ImGui::TableNextColumn(); W(); changed |= noneNormalsTexture.renderGUI("##noneNormalsTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("None Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= redTexture.renderGUI("##redTexture");
			ImGui::TableNextColumn(); W(); changed |= redNormalsTexture.renderGUI("##redNormalsTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Red Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= greenTexture.renderGUI("##greenTexture");
			ImGui::TableNextColumn(); W(); changed |= greenNormalsTexture.renderGUI("##greenNormalsTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Green Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= blueTexture.renderGUI("##blueTexture");
			ImGui::TableNextColumn(); W(); changed |= blueNormalsTexture.renderGUI("##blueNormalsTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Blue Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Scale", &scale, 0.0f, 100.0f, "%.1f");
			ImGui::TableNextColumn();
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtBlendMapMaterialClass::serialize
//

nlohmann::json OtBlendMapMaterialClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data.update(OtMaterialClass::serialize(basedir));

	data["blendMap"] = OtPathGetRelative(blendMapTexture.getPath(), basedir);

	data["none"] = OtPathGetRelative(noneTexture.getPath(), basedir);
	data["red"] = OtPathGetRelative(redTexture.getPath(), basedir);
	data["green"] = OtPathGetRelative(greenTexture.getPath(), basedir);
	data["blue"] = OtPathGetRelative(blueTexture.getPath(), basedir);

	data["noneNormals"] = OtPathGetRelative(noneNormalsTexture.getPath(), basedir);
	data["redNormals"] = OtPathGetRelative(redNormalsTexture.getPath(), basedir);
	data["greenNormals"] = OtPathGetRelative(greenNormalsTexture.getPath(), basedir);
	data["blueNormals"] = OtPathGetRelative(blueNormalsTexture.getPath(), basedir);

	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["scale"] = scale;

	return data;
}


//
//	OtBlendMapMaterialClass::deserialize
//

void OtBlendMapMaterialClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtMaterialClass::deserialize(data, basedir);
	blendMapTexture = OtPathGetAbsolute(data, "blendMap", basedir);

	noneTexture = OtPathGetAbsolute(data, "none", basedir);
	redTexture = OtPathGetAbsolute(data, "red", basedir);
	greenTexture = OtPathGetAbsolute(data, "green", basedir);
	blueTexture = OtPathGetAbsolute(data, "blue", basedir);

	noneNormalsTexture = OtPathGetAbsolute(data, "noneNormals", basedir);
	redNormalsTexture = OtPathGetAbsolute(data, "redNormals", basedir);
	greenNormalsTexture = OtPathGetAbsolute(data, "greenNormals", basedir);
	blueNormalsTexture = OtPathGetAbsolute(data, "blueNormals", basedir);

	metallic = data.value("metallic", 0.5f);
	roughness = data.value("roughness", 0.5f);
	scale = data.value("scale", 1.0f);
}


//
//	OtBlendMapMaterialClass::getMeta
//

OtType OtBlendMapMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtBlendMapMaterialClass>("BlendMapMaterial", OtMaterialClass::getMeta());
	}

	return type;
}
