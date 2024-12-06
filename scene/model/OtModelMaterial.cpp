//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtPath.h"

#include "OtModelMaterial.h"


//
//	OtModelMaterial::load
//

void OtModelMaterial::load(const aiMaterial* mat, const std::string& dir) {
	// work variables
	aiColor4D color;
	aiString string;
	ai_real real;

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
		auto file = std::string(string.data, string.length);
		material->setAlbedoTexture(OtPath::join(dir, file));

	} else if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &string) == AI_SUCCESS) {
		auto file = std::string(string.data, string.length);
		material->setAlbedoTexture(OtPath::join(dir, file));
	}

	// normal texture
	if (mat->GetTexture(aiTextureType_NORMALS, 0, &string) == AI_SUCCESS) {
		auto file = std::string(string.data, string.length);
		material->setNormalTexture(OtPath::join(dir, file));
	}

	// metallic texture
	if (mat->GetTexture(aiTextureType_METALNESS, 0, &string) == AI_SUCCESS) {
		auto file = std::string(string.data, string.length);
		material->setMetallicRoughnessTexture(OtPath::join(dir, file));
	}

	// emissive texture
	if (mat->GetTexture(aiTextureType_EMISSIVE, 0, &string) == AI_SUCCESS) {
		auto file = std::string(string.data, string.length);
		material->setEmissiveTexture(OtPath::join(dir, file));
	}

	// ambient occlusion texture
	if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &string) == AI_SUCCESS) {
		auto file = std::string(string.data, string.length);
		material->setAoTexture(OtPath::join(dir, file));
	}

	valid = true;
}
