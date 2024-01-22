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
#include <vector>

#include "OtAsset.h"
#include "OtAssetBase.h"

#include "OtModelMaterial.h"
#include "OtModelMesh.h"

//
//	OtModelAsset
//

class OtModelAsset : public OtAssetBase {
public:
	// load the model
	bool load() override;

	// access the meshes and materials
	inline std::vector<OtModelMesh>& getMeshes() { return meshes; }
	inline std::vector<OtModelMaterial>& getMaterials() { return materials; }

	// access bounding box
	OtAABB& getAABB() { return aabb; }

private:
	// our meshes and materials
	std::vector<OtModelMesh> meshes;
	std::vector<OtModelMaterial> materials;

	// model bounding box
	OtAABB aabb;
};
