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

#include "OtMaterial.h"


//
//	OtMaterial
//

class OtPbrMaterialClass;
using OtPbrMaterial = OtObjectPointer<OtPbrMaterialClass>;

class OtPbrMaterialClass : public OtMaterialClass {
public:
	// update material
	void update() override;

	// GUI to change geometry properties
	bool renderGUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// material name
	static constexpr char const* name = "PBR";

	// get type definition
	static OtType getMeta();

private:
	// the scene renderer need access to our properties
	friend class OtSceneRenderer;

	// stored properties
	glm::vec4 albedo{1.0f};
	float metallic = 0.5f;
	float roughness = 0.5f;
	float emissive = 0.0f;
	float ao = 1.0f;

	std::filesystem::path albedoTexturePath;
	std::filesystem::path normalTexturePath;
	std::filesystem::path metallicTexturePath;
	std::filesystem::path roughnessTexturePath;
	std::filesystem::path emissiveTexturePath;
	std::filesystem::path aoTexturePath;

	// runtime properties
	OtTexture albedoTexture;
	OtTexture normalTexture;
	OtTexture metallicTexture;
	OtTexture roughnessTexture;
	OtTexture emissiveTexture;
	OtTexture aoTexture;

	bool updateAlbedoTexture = false;
	bool updateNormalTexture = false;
	bool updateMetallicTexture = false;
	bool updateRoughnessTexture = false;
	bool updateEmissiveTexture = false;
	bool updateAoTexture = false;
};
