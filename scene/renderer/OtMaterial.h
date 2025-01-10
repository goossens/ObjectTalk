//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtAsset.h"
#include "OtTextureAsset.h"


//
//	OtMaterial
//

class OtMaterial {
public:
	// UI to change material properties
	bool renderUI();

	// (de)serialize material
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// set the properties
	void setAlbedo(const glm::vec4& a) { albedo = a; }
	void setMetallic(float m) { metallic = m; }
	void setRoughness(float r) { roughness = r; }
	void setEmissive(const glm::vec3& e) { emissive = e; }
	void setAo(float a) { ao = a; }

	// set the textures
	void setAlbedoTexture(const std::string& path) { albedoTexture = path; }
	void setNormalTexture(const std::string& path) { normalTexture = path; }
	void setMetallicRoughnessTexture(const std::string& path) { metallicRoughnessTexture = path; }
	void setEmissiveTexture(const std::string& path) { emissiveTexture = path; }
	void setAoTexture(const std::string& path) { aoTexture = path; }

private:
	// the scene renderer needs access to our properties
	friend class OtSceneRenderPass;

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
