//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtTexture.h"


//
//	OtMaterialComponent
//

class OtMaterialComponent {
public:
	// GUI to change geometry properties
	bool renderGUI();

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Material";

	// update material (if required)
	void update();

	// stored properties
	glm::vec4 albedo{1.0f};
	float metallic = 0.5f;
	float roughness = 0.5f;
	float ao = 1.0f;
	std::filesystem::path albedoTexturePath;
	std::filesystem::path normalTexturePath;
	std::filesystem::path metallicTexturePath;
	std::filesystem::path roughnessTexturePath;
	std::filesystem::path aoTexturePath;

	// runtime properties
	OtTexture albedoTexture;
	OtTexture normalTexture;
	OtTexture metallicTexture;
	OtTexture roughnessTexture;
	OtTexture aoTexture;

	bool updateAlbedoTexture = false;
	bool updateNormalTexture = false;
	bool updateMetallicTexture = false;
	bool updateRoughnessTexture = false;
	bool updateAoTexture = false;
};
