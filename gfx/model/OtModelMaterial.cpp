//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "assimp/texture.h"
#include "fmt/format.h"
#include "glm/glm.hpp"

#include "OtPath.h"
#include "OtText.h"

#include "OtModelMaterial.h"


//
//	OtModelMaterial::load
//

void OtModelMaterial::load(size_t modelID, const aiScene* scene, const aiMaterial* mat, const std::string& dir) {
	// work variables
	aiColor4D color;
	aiString string;
	ai_real real;

	// save name
	name = std::string(mat->GetName().C_Str());

	// create new material
	material = std::make_shared<OtMaterial>();

	// albedo color
	if (mat->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS) {
		material->setAlbedo(glm::vec4(color.r, color.g, color.b, color.a));

	} else if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		material->setAlbedo(glm::vec4(color.r, color.g, color.b, color.a));
	}

	// metallic factor
	if (mat->Get(AI_MATKEY_METALLIC_FACTOR, real) == AI_SUCCESS) {
		material->setMetallic(real);
	}

	// roughness factor
	if (mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, real) == AI_SUCCESS) {
		material->setRoughness(real);
	}

	// emissive factor
	if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
		material->setEmissive(glm::vec3(color.r, color.g, color.b));
	}

	// albedo texture
	if (mat->GetTexture(aiTextureType_BASE_COLOR, 0, &string) == AI_SUCCESS) {
		auto path = getTexturePath(modelID, scene, dir, std::string(string.data, string.length));
		material->setAlbedoTexture(path);

	} else if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &string) == AI_SUCCESS) {
		auto path = getTexturePath(modelID, scene, dir, std::string(string.data, string.length));
		material->setAlbedoTexture(path);
	}

	// normal texture
	if (mat->GetTexture(aiTextureType_NORMALS, 0, &string) == AI_SUCCESS) {
		auto path = getTexturePath(modelID, scene, dir, std::string(string.data, string.length));
		material->setNormalTexture(path);
	}

	// metallic texture
	if (mat->GetTexture(aiTextureType_METALNESS, 0, &string) == AI_SUCCESS) {
		auto path = getTexturePath(modelID, scene, dir, std::string(string.data, string.length));
		material->setMetallicRoughnessTexture(path);
	}

	// emissive texture
	if (mat->GetTexture(aiTextureType_EMISSIVE, 0, &string) == AI_SUCCESS) {
		auto path = getTexturePath(modelID, scene, dir, std::string(string.data, string.length));
		material->setEmissiveTexture(path);
	}

	// ambient occlusion texture
	if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &string) == AI_SUCCESS) {
		auto path = getTexturePath(modelID, scene, dir, std::string(string.data, string.length));
		material->setAoTexture(path);
	}

	valid = true;
}


//
//	OtModelMaterial::getTexturePath
//

std::string OtModelMaterial::getTexturePath(size_t modelID, const aiScene* scene, const std::string& dir, const std::string& file) {
	auto [texture, textureID] = scene->GetEmbeddedTextureAndIndex(file.c_str());

	if (textureID != -1) {
		return fmt::format("virtual:model{}.{}", modelID, textureID);

	} else {
		return OtPath::join(dir, file);
	}
}
