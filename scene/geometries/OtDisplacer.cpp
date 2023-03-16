//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "FastNoiseLite.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtDisplacer.h"


//
//	OtDisplacer::displace
//

void OtDisplacer::displace(std::vector<OtVertex> &vertices, std::vector<uint32_t> &triangles) {
	// reset vertices and AABB
	displaced.clear();
	displaced.reserve(vertices.size());
	aabb.clear();

	// use displacement by heightmap or by noise
	if (state == Map && heightmap.isValid()) {
		displaceByMap(vertices, triangles);

	} else if (state == Noise) {
		displaceByNoise(vertices, triangles);
	}
}


//
//	OtDisplacer::renderGUI
//

bool OtDisplacer::renderGUI() {
	bool changed = false;
	float itemWidth = ImGui::CalcItemWidth();

	// render button with status
	auto label = (state == Off) ? "Off" : (state == Map) ? "Map" : "Noise";

	if (ImGui::Button(label, ImVec2(itemWidth, 0.0f))) {
		ImGui::OpenPopup("DisplacementPopup");
	}

	// render label
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Displacement");

	// open popup (if required)
	if (ImGui::BeginPopup("DisplacementPopup")) {
		if (ImGui::RadioButton("Off", state == Off)) { state = Off; changed = true; }
		ImGui::SameLine();
		if (ImGui::RadioButton("Map", state == Map)) { state = Map; changed = true; }
		ImGui::SameLine();
		if (ImGui::RadioButton("Noise", state == Noise)) { state = Noise; changed = true; }

		ImGui::BeginDisabled(state != Map);
		if (OtUiFileSelector("Map", map)) { updateHeightmap(); changed = true; }
		ImGui::EndDisabled();

		ImGui::BeginDisabled(state != Noise);
		changed |= ImGui::InputInt("Seed", &seed);
		changed |= ImGui::InputInt("Octaves", &octaves);
		changed |= ImGui::DragFloat("Frequency", &frequency, 0.1f, 0.1f, 40.0f);
		changed |= ImGui::DragFloat("Lacunarity", &lacunarity, 0.01f, 1.0f, 2.0f);
		changed |= ImGui::DragFloat("Persistance", &persistance, 0.01f, 0.1f, 1.0f);
		ImGui::EndDisabled();

		changed |= ImGui::DragFloat("Scale", &scale, 0.01f, 0.01f, 1.0f);
		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtDisplacer::serialize
//

nlohmann::json OtDisplacer::serialize() {
	auto data = nlohmann::json::object();
	data["state"] = state;
	data["map"] = map;
	data["seed"] = seed;
	data["octaves"] = octaves;
	data["frequency"] = frequency;
	data["lacunarity"] = lacunarity;
	data["persistance"] = persistance;
	data["scale"] = scale;
	return data;
}


//
//	OtDisplacer::deserialize
//

void OtDisplacer::deserialize(nlohmann::json data) {
	auto oldmap = map;
	state = data.value("state", Off);
	map = data.value("map", "");
	seed = data.value("seed", 1337);
	octaves = data.value("octaves", 3);
	frequency = data.value("frequency", 2.0f);
	lacunarity = data.value("lacunarity", 2.0f);
	persistance = data.value("persistance", 0.5f);
	scale = data.value("scale", 0.5f);

	if (map != oldmap && std::filesystem::is_regular_file(map)) {
		updateHeightmap();
	}
}


//
//	OtDisplacer::displaceByMap
//

void OtDisplacer::displaceByMap(std::vector<OtVertex> &vertices, std::vector<uint32_t> &triangles) {
	// apply hieghtmap to all vertices
	for (auto& vertex : vertices) {
		OtVertex& newVertex = displaced.emplace_back(vertex);
		auto offset = heightmap.getHeight(newVertex.uv.x, newVertex.uv.y) * scale;
		newVertex.position += newVertex.normal * offset;
		newVertex.normal = glm::vec3(0.0f);
		aabb.addPoint(newVertex.position);
	}

	// (re)calculate all normals
	reclaculateNormals(triangles);
}


//
//	OtDisplacer::displaceByNoise
//

void OtDisplacer::displaceByNoise(std::vector<OtVertex> &vertices, std::vector<uint32_t> &triangles) {
	// apply noise to all vertices
	FastNoiseLite noise;
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	noise.SetSeed(seed);
	noise.SetFrequency(frequency);
	noise.SetFractalOctaves(octaves);
	noise.SetFractalLacunarity(lacunarity);
	noise.SetFractalGain(persistance);

	for (auto& vertex : vertices) {
		OtVertex& newVertex = displaced.emplace_back(vertex);
		auto uv = newVertex.uv * 2.0f - glm::vec2(1.0f);
		auto offset = noise.GetNoise(uv.x, uv.y) * scale;
		newVertex.position += newVertex.normal * offset;
		newVertex.normal = glm::vec3(0.0f);
		aabb.addPoint(newVertex.position);
	}

	// (re)calculate all normals
	reclaculateNormals(triangles);
}


//
//	OtDisplacer::reclaculateNormals
//

void OtDisplacer::reclaculateNormals(std::vector<uint32_t>& triangles) {
	for (auto c = 0; c < triangles.size(); c += 3) {
		OtVertex& v0 = displaced[triangles[c]];
		OtVertex& v1 = displaced[triangles[c + 1]];
		OtVertex& v2 = displaced[triangles[c + 2]];

		auto normal = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
		v0.normal += normal;
		v1.normal += normal;
		v2.normal += normal;
	}

	for (auto& vertex : displaced) {
		vertex.normal = glm::normalize(vertex.normal);
	}
}


//
//	OtDisplacer::updateHeightmap
//

void OtDisplacer::updateHeightmap() {
	if (map.empty()) {
		heightmap.clear();

	} else {
		heightmap.load(map);
	}
}
