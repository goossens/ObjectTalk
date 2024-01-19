//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//


#include <string>

#include "assimp/material.h"

#include "OtPbrMaterial.h"


//
//	OtModelMaterial
//

class OtModelMaterial {
public:
	// constructor
	OtModelMaterial();

	// clear the material
	void clear();

	// load the material
	void load(const aiMaterial* material, const std::string& dir);

	// see if material is valid
	inline bool isValid() { return valid; }

	// access the material
	inline OtPbrMaterial& getPbrMaterial() { return material; }

private:
	// the PBR material
	OtPbrMaterial material;
	bool valid = false;
};
