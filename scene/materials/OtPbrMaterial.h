//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtAsset.h"
#include "OtTextureAsset.h"

#include "OtMaterial.h"


//
//	OtMaterial
//

class OtPbrMaterialClass;
using OtPbrMaterial = OtObjectPointer<OtPbrMaterialClass>;

class OtPbrMaterialClass : public OtMaterialClass {
public:
	// UI to change material properties
	bool renderUI() override;

	// (de)serialize material
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of material
	const char* getTypeName() override { return name; }

	// material name
	static constexpr char const* name = "PBR";

	// set the properties
	void setAlbedo(OtObject object);
	void setAlbedoVector(const glm::vec4& a) { albedo = a; }
	void setMetallic(float m) { metallic = m; }
	void setRoughness(float r) { roughness = r; }
	void setEmissive(OtObject object);
	void setEmissiveVector(const glm::vec3& e) { emissive = e; }
	void setAo(float a) { ao = a; }

	// set the textures
	void setAlbedoTexture(const std::filesystem::path& path) { albedoTexture = path; }
	void setNormalTexture(const std::filesystem::path& path) { normalTexture = path; }
	void setMetallicRoughnessTexture(const std::filesystem::path& path) { metallicRoughnessTexture = path; }
	void setEmissiveTexture(const std::filesystem::path& path) { emissiveTexture = path; }
	void setAoTexture(const std::filesystem::path& path) { aoTexture = path; }

	// get type definition
	static OtType getMeta();

private:
	// the scene renderer needs access to our properties
	friend class OtSceneRenderer;

	// stored properties
	glm::vec4 albedo{1.0f};
	float metallic = 0.5f;
	float roughness = 0.5f;
	glm::vec3 emissive{0.0f};
	float ao = 1.0f;
	float scale = 1.0f;

	OtAsset<OtTextureAsset> albedoTexture;
	OtAsset<OtTextureAsset> normalTexture;
	OtAsset<OtTextureAsset> metallicRoughnessTexture;
	OtAsset<OtTextureAsset> emissiveTexture;
	OtAsset<OtTextureAsset> aoTexture;
};
