//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "OtException.h"

#include "OtModel.h"


//
//	OtModel::clear
//

void OtModel::clear() {
	meshes.clear();
	materials.clear();
	valid = false;
}


//
//	OtModel::load
//

void OtModel::load(const std::filesystem::path& path) {
	// clear the current data
	clear();

	// remember the path
	modelPath = path;

	// create an asset importer
	Assimp::Importer importer;

	// determine the import flags
	static constexpr uint32_t flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_FlipUVs;

	// read the model file
	const aiScene* scene = importer.ReadFile(modelPath.string(), flags);

	// ensure model was loaded correctly
	if (scene == nullptr) {
		OtExcept("Unable to load model [%s], error: %s", modelPath.c_str(), importer.GetErrorString());
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtExcept("Incomplete model [%s]", modelPath.c_str());
	}

	// load all the meshes
	meshes.resize(scene->mNumMeshes);

	for (auto i = 0; i < scene->mNumMeshes; i++) {
		meshes[i].load(scene->mMeshes[i]);
	}

	// load all the materials
	materials.resize(scene->mNumMaterials);
	auto dir = path.parent_path();

	for (auto i = 0; i < scene->mNumMaterials; i++) {
		materials[i].load(scene->mMaterials[i], dir);
	}

	// we're now ready to go
	valid = true;
}
