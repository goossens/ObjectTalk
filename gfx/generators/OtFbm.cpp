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

#include "OtFbm.h"


//
//	OtFbm::renderGUI
//

bool OtFbm::renderGUI() {
	bool changed = false;

	changed |= ImGui::SliderInt("Frequency", &frequency, 2, 20);
	changed |= ImGui::SliderInt("Lacunarity", &lacunarity, 1, 10);
	changed |= ImGui::DragFloat("Amplitude", &amplitude, 0.1f, 0.1f, 1.0f);
	changed |= ImGui::DragFloat("Persistence", &persistence, 0.1f, 0.1f, 1.0f);
	changed |= ImGui::DragInt("Octaves", &octaves, 1, 1, 10);

	return changed;
}


//
//	OtFbm::serialize
//

nlohmann::json OtFbm::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["frequency"] = frequency;
	data["lacunarity"] = lacunarity;
	data["amplitude"] = amplitude;
	data["persistence"] = persistence;
	data["octaves"] = octaves;
	return data;
}


//
//	OtFbm::deserialize
//

void OtFbm::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	frequency = data.value("frequency", 10);
	lacunarity = data.value("lacunarity", 2);
	amplitude = data.value("amplitude", 0.5f);
	octaves = data.value("octaves", 5);
}


//
//	OtFbm::execute
//

void OtFbm::execute(OtPass& pass, int w, int h) {
	uniform.set(0, glm::vec4(frequency, lacunarity, amplitude, persistence));
	uniform.set(1, glm::vec4(octaves, 0.0f, 0.0f, 0.0f));
	uniform.submit();
	pass.runShaderProgram(program);
}
