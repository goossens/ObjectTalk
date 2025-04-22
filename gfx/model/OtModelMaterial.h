//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//


#include <memory>
#include <string>

#include "assimp/material.h"
#include "assimp/scene.h"

#include "OtMaterial.h"


//
//	OtModelMaterial
//

class OtModelMaterial {
public:
	// load the material
	void load(size_t modelID, const aiScene* scene, const aiMaterial* material, const std::string& dir);

	// see if material is valid
	inline bool isValid() { return valid; }

	// access the material
	inline std::shared_ptr<OtMaterial> getMaterial() { return material; }

private:
	// the PBR material
	std::shared_ptr<OtMaterial> material;
	bool valid = false;

	// determine texture path
	std::string getTexturePath(size_t modelID, const aiScene* scene, const std::string& dir, const std::string& file);
};
