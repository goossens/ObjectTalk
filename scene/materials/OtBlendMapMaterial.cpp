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
//	OtBlendMapMaterialClass::void OtBlendMapMaterialClass::update() {
//

static inline void updateTexture(OtTexture& texture, const std::filesystem::path& path, bool& flag) {
	if (flag) {
		if (std::filesystem::is_regular_file(path)) {
			texture.loadFromFile(path.string(), true);

		} else {
			texture.clear();
		}

		flag = false;
	}
}

void OtBlendMapMaterialClass::update() {
	updateTexture(blendMapTexture, blendMapPath, updateBlendMapTexture);

	updateTexture(noneTexture, nonePath, updateNoneTexture);
	updateTexture(redTexture, redPath, updateRedTexture);
	updateTexture(greenTexture, greenPath, updateGreenTexture);
	updateTexture(blueTexture, bluePath, updateBlueTexture);

	updateTexture(normalsNoneTexture, normalsNonePath, updateNormalsNoneTexture);
	updateTexture(normalsRedTexture, normalsRedPath, updateNormalsRedTexture);
	updateTexture(normalsGreenTexture, normalsGreenPath, updateNormalsGreenTexture);
	updateTexture(normalsBlueTexture, normalsBluePath, updateNormalsBlueTexture);
}


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
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##blendMapTexture", blendMapPath, updateBlendMapTexture);
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("BlendMap");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##noneTexture", nonePath, updateNoneTexture);
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##noneNormals", normalsNonePath, updateNormalsNoneTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("None Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##redTexture", redPath, updateRedTexture);
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##redNormals", normalsRedPath, updateNormalsRedTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Red Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##greenTexture", greenPath, updateGreenTexture);
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##greenNormals", normalsGreenPath, updateNormalsGreenTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Green Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##blueTexture", bluePath, updateBlueTexture);
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##blueNormals", normalsBluePath, updateNormalsBlueTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Blue Color");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Emissive", &emissive, 0.0f, 4.0f, "%.2f");
			ImGui::TableNextColumn(); changed |= ImGui::SliderFloat("Scale", &scale, 0.0f, 100.0f, "%.1f");
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	if (changed) {
		update();
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

	data["blendMapPath"] = OtPathGetRelative(blendMapPath, basedir);

	data["nonePath"] = OtPathGetRelative(nonePath, basedir);
	data["redPath"] = OtPathGetRelative(redPath, basedir);
	data["greenPath"] = OtPathGetRelative(greenPath, basedir);
	data["bluePath"] = OtPathGetRelative(bluePath, basedir);

	data["normalsNonePath"] = OtPathGetRelative(normalsNonePath, basedir);
	data["normalsRedPath"] = OtPathGetRelative(normalsRedPath, basedir);
	data["normalsGreenPath"] = OtPathGetRelative(normalsGreenPath, basedir);
	data["normalsBluePath"] = OtPathGetRelative(normalsBluePath, basedir);

	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["emissive"] = emissive;
	data["scale"] = scale;

	return data;
}


//
//	OtBlendMapMaterialClass::deserialize
//

void OtBlendMapMaterialClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtMaterialClass::deserialize(data, basedir);
	blendMapPath = OtPathGetAbsolute(data, "blendMapPath", basedir);

	nonePath = OtPathGetAbsolute(data, "nonePath", basedir);
	redPath = OtPathGetAbsolute(data, "redPath", basedir);
	greenPath = OtPathGetAbsolute(data, "greenPath", basedir);
	bluePath = OtPathGetAbsolute(data, "bluePath", basedir);

	normalsNonePath = OtPathGetAbsolute(data, "normalsNonePath", basedir);
	normalsRedPath = OtPathGetAbsolute(data, "normalsRedPath", basedir);
	normalsGreenPath = OtPathGetAbsolute(data, "normalsGreenPath", basedir);
	normalsBluePath = OtPathGetAbsolute(data, "normalsBluePath", basedir);

	metallic = data.value("metallic", 0.5f);
	roughness = data.value("roughness", 0.5f);
	emissive = data.value("emissive", 0.0f);
	scale = data.value("scale", 1.0f);

	updateBlendMapTexture = !blendMapPath.empty();

	updateNoneTexture = !nonePath.empty();
	updateRedTexture = !redPath.empty();
	updateGreenTexture = !greenPath.empty();
	updateBlueTexture = !bluePath.empty();

	updateNormalsNoneTexture = !normalsNonePath.empty();
	updateNormalsRedTexture = !normalsRedPath.empty();
	updateNormalsGreenTexture = !normalsGreenPath.empty();
	updateNormalsBlueTexture = !normalsBluePath.empty();
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
