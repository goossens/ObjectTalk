//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "OtFormat.h"
#include "OtLog.h"

#include "OtPathTools.h"

#include "OtModelAsset.h"


//
//	OtModelAsset::load
//

bool OtModelAsset::load() {
	// clear the current data
	meshes.clear();
	materials.clear();
	aabb.clear();

	// create an asset importer
	Assimp::Importer importer;

	// determine the import flags
	static constexpr uint32_t flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_GenBoundingBoxes |
		aiProcess_FlipUVs;

	// read the model file
	const aiScene* scene = importer.ReadFile(path, flags);

	// ensure model was loaded correctly
	if (scene == nullptr) {
		OtLogWarning(OtFormat("Unable to load model [%s], error: %s", path.c_str(), importer.GetErrorString()));
		return false;
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtLogWarning(OtFormat("Incomplete model [%s]", path.c_str()));
		return false;
	}

	// load all the meshes
	meshes.resize(scene->mNumMeshes);

	for (auto i = 0; i < scene->mNumMeshes; i++) {
		meshes[i].load(scene->mMeshes[i]);
		aabb.addAABB(meshes[i].getAABB());
	}

	// load all the materials
	materials.resize(scene->mNumMaterials);
	auto dir = OtPathGetParent(path);

	for (auto i = 0; i < scene->mNumMaterials; i++) {
		materials[i].load(scene->mMaterials[i], dir);
	}

	return true;
}
