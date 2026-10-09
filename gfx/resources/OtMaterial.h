//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp" // IWYU pragma: keep
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
	void deserialize(nlohmann::json& data, std::string* basedir);

	// access the properties
	inline void setAlbedo(const glm::vec4& value) { albedo = value; }
	inline void setMetallic(float value) { metallic = value; }
	inline void setRoughness(float value) { roughness = value; }
	inline void setEmissive(const glm::vec3& value) { emissive = value; }
	inline void setAo(float value) { ao = value; }
	inline void setTextureOffset(const glm::vec2& value) { offset = value; }
	inline void setTextureScale(float value) { scale = value; }

	inline glm::vec4 getAlbedo() { return albedo; }
	inline float getMetallic() { return metallic; }
	inline float getRoughness() { return roughness; }
	inline glm::vec3 getEmissive() { return emissive; }
	inline float getAo() { return ao; }
	inline glm::vec2 getTextureOffset() { return offset; }
	inline float getTextureScale() { return scale; }

	// set the textures
	inline void setAlbedoTexture(const std::string& path) { albedoTexture = path; }
	inline void setNormalTexture(const std::string& path) { normalTexture = path; }
	inline void setMetallicRoughnessTexture(const std::string& path) { metallicRoughnessTexture = path; }
	inline void setEmissiveTexture(const std::string& path) { emissiveTexture = path; }
	inline void setAoTexture(const std::string& path) { aoTexture = path; }

	inline std::string getAlbedoTexture() { return albedoTexture.getPath(); }
	inline std::string getNormalTexture() { return normalTexture.getPath(); }
	inline std::string getMetallicRoughnessTexture() { return metallicRoughnessTexture.getPath(); }
	inline std::string getEmissiveTexture() { return emissiveTexture.getPath(); }
	inline std::string getAoTexture() { return aoTexture.getPath(); }

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
