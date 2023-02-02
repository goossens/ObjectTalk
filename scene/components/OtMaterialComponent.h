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

	// see if material is valid
	bool isValid();

	// stored properties
	glm::vec4 albedo{1.0f};
	float metalic = 0.0f;
	float roughness = 0.5f;
	float ao = 0.0f;
	std::filesystem::path albedoTexturePath;
	std::filesystem::path normalTexturePath;
	std::filesystem::path metalicTexturePath;
	std::filesystem::path roughnessTexturePath;
	std::filesystem::path aoTexturePath;

	// runtime properties
	bool update = false;
	OtTexture albedoTexture;
	OtTexture normalTexture;
	OtTexture metalicTexture;
	OtTexture roughnessTexture;
	OtTexture aoTexture;

private:
	// helper functions
	bool editPath(const char* label, std::filesystem::path& path, OtTexture& texture);
};
