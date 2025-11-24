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
	inline void setAlbedo(const glm::vec4& a) { albedo = a; }
	inline void setMetallic(float m) { metallic = m; }
	inline void setRoughness(float r) { roughness = r; }
	inline void setEmissive(const glm::vec3& e) { emissive = e; }
	inline void setAo(float a) { ao = a; }
	inline void setTextureOffset(const glm::vec2& o) { offset = o; }
	inline void setTextureScale(float s) { scale = s; }

	// set the textures
	inline void setAlbedoTexture(const std::string& path) { albedoTexture = path; }
	inline void setNormalTexture(const std::string& path) { normalTexture = path; }
	inline void setMetallicRoughnessTexture(const std::string& path) { metallicRoughnessTexture = path; }
	inline void setEmissiveTexture(const std::string& path) { emissiveTexture = path; }
	inline void setAoTexture(const std::string& path) { aoTexture = path; }

private:
	// the scene renderer needs access to our properties
	friend class OtSceneRenderEntitiesPass;

	// stored properties
	glm::vec4 albedo{1.0f};
	float metallic = 0.5f;
	float roughness = 0.5f;
	glm::vec3 emissive{0.0f};
	float ao = 1.0f;
	glm::vec2 offset{0.0f};
	float scale = 1.0f;

	OtAsset<OtTextureAsset> albedoTexture;
	OtAsset<OtTextureAsset> normalTexture;
	OtAsset<OtTextureAsset> metallicRoughnessTexture;
	OtAsset<OtTextureAsset> emissiveTexture;
	OtAsset<OtTextureAsset> aoTexture;
};
