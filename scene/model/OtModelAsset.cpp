//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "OtLog.h"

#include "OtGpu.h"
#include "OtPath.h"

#include "OtModelAsset.h"


//
//	OtModelAsset::load
//

OtAssetBase::State OtModelAsset::load() {
	// clear the current data
	meshes.clear();
	materials.clear();
	aabb.clear();

	// create an asset importer
	Assimp::Importer importer;

	// determine the import flags
	uint32_t flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_GenBoundingBoxes |
		(OtGpuHasOriginBottomLeft() ? 0 : aiProcess_FlipUVs);

	// read the model file
	const aiScene* scene = importer.ReadFile(path, flags);

	// ensure model was loaded correctly
	if (scene == nullptr) {
		OtLogWarning("Unable to load model [{}], error: {}", path, importer.GetErrorString());
		return State::invalid;
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtLogWarning("Incomplete model [{}]", path);
		return State::invalid;
	}

	// load all the meshes
	meshes.resize(scene->mNumMeshes);

	for (auto i = 0; i < scene->mNumMeshes; i++) {
		meshes[i].load(scene->mMeshes[i]);
		aabb.addAABB(meshes[i].getAABB());
	}

	// load all the materials
	materials.resize(scene->mNumMaterials);
	auto dir = OtPath::getParent(path);

	for (auto i = 0; i < scene->mNumMaterials; i++) {
		materials[i].load(scene->mMaterials[i], dir);
	}

	return State::ready;
}
