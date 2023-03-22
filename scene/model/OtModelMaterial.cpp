//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtException.h"

#include "OtModelMaterial.h"


//
//	Helper functions
//

static inline glm::vec2 ToVec2(const aiVector3D& v) {
	return glm::vec2(v.x, v.y);
}

static inline glm::vec3 ToVec3(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}

//
//	OtModelMaterial::OtModelMaterial
//

OtModelMaterial::OtModelMaterial() {
	material = OtPbrMaterial::create();
}


//
//	OtModelMaterial::clear
//

void OtModelMaterial::clear() {
	material = nullptr;
	valid = false;
}


//
//	OtModelMaterial::load
//

void OtModelMaterial::load(const aiMaterial* mat, const std::filesystem::path& dir) {
	// work variables
	aiColor4D color;
	aiString string;
	ai_real real;

	// albedo
	if (mat->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS) {
		material->albedo = glm::vec4(color.r, color.g, color.b, color.a);
	}

	// metallic
	if (mat->Get(AI_MATKEY_METALLIC_FACTOR, real) == AI_SUCCESS) {
		material->metallic = real;
	}

	// roughness
	if (mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, real) == AI_SUCCESS) {
		material->roughness = real;
	}

	// albedo texture
	mat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &string);

	if (string.length) {
		auto file = std::filesystem::path(std::string(string.data, string.length));
		material->setAlbedoTexture(std::filesystem::canonical(dir / file));
	}

	// normal texture
	mat->GetTexture(aiTextureType_NORMALS, 0, &string);

	if (string.length) {
		auto file = std::filesystem::path(std::string(string.data, string.length));
		material->setNormalTexture(std::filesystem::canonical(dir / file));
	}

	// metallic texture
	mat->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &string);

	if (string.length) {
		auto file = std::filesystem::path(std::string(string.data, string.length));
		material->setMetallicTexture(std::filesystem::canonical(dir / file));
	}

	// roughness texture
	mat->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &string);

	if (string.length) {
		auto file = std::filesystem::path(std::string(string.data, string.length));
		material->setRoughnessTexture(std::filesystem::canonical(dir / file));
	}

	// emissive texture
	mat->GetTexture(aiTextureType_EMISSIVE, 0, &string);

	if (string.length) {
		auto file = std::filesystem::path(std::string(string.data, string.length));
		material->setEmissiveTexture(std::filesystem::canonical(dir / file));
	}

	// ambient occlusion texture
	mat->GetTexture(aiTextureType_LIGHTMAP, 0, &string);

	if (string.length) {
		auto file = std::filesystem::path(std::string(string.data, string.length));
		material->setAoTexture(std::filesystem::canonical(dir / file));
	}

	material->update();
	valid = true;
}
