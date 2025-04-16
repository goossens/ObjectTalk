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
#include <vector>

#include "OtAsset.h"
#include "OtAssetBase.h"

#include "OtModelAnimation.h"
#include "OtModelMaterial.h"
#include "OtModelMesh.h"
#include "OtModelNodes.h"
#include "OtModelTexture.h"

//
//	OtModelAsset
//

class OtModelAsset : public OtAssetBase {
public:
	// access the meshes and materials
	inline std::vector<OtModelMesh>& getMeshes() { return meshes; }
	inline std::vector<OtModelMaterial>& getMaterials() { return materials; }

	// access bounding box
	OtAABB& getAABB() { return aabb; }

	// asset properties
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".fbx,.glb,.gltf,.md5mesh,.obj";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the model
	OtAssetBase::State load() override;

private:
	// our nodes, meshes, materials, textures and animations
	OtModelNodes nodes;
	std::vector<OtModelMesh> meshes;
	std::vector<OtModelMaterial> materials;
	std::vector<OtModelTexture> textures;
	std::vector<OtModelAnimation> animations;

	// model bounding box
	OtAABB aabb;

	// model identifier
	size_t id;
};
