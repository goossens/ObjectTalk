//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <atomic>
#include <cstdint>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "OtException.h"
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

	// assign a model identifier
	static std::atomic<size_t> nextID = 0;
	id = nextID++;

	// create an asset importer
	Assimp::Importer importer;

	// determine the import flags
	auto flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_GenBoundingBoxes;

	if (!OtGpuHasOriginBottomLeft()) {
		flags |= aiProcess_FlipUVs;
	}

	// read the model file
	const aiScene* scene = importer.ReadFile(path, static_cast<unsigned int>(flags));

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

	try {
		// load the node hierarchy
		nodes.load(scene->mRootNode);

		// load all the meshes
		if (scene->HasMeshes()) {
			meshes.resize(scene->mNumMeshes);

			for (auto i = 0u; i < scene->mNumMeshes; i++) {
				meshes[i].load(scene->mMeshes[i], nodes);
				aabb.addAABB(meshes[i].getAABB());
			}
		}

		// load all the embedded textures
		if (scene->HasTextures()) {
			textures.resize(scene->mNumTextures);

			for (auto i = 0u; i < scene->mNumTextures; i++) {
				textures[i].load(id, i, scene->mTextures[i]);
			}
		}

		// load all the materials
		if (scene->HasMaterials()) {
			materials.resize(scene->mNumMaterials);
			auto dir = OtPath::getParent(path);

			for (auto i = 0u; i < scene->mNumMaterials; i++) {
				materials[i].load(id, scene, scene->mMaterials[i], dir);
			}
		}

		// load the animations
		if (scene->HasAnimations()) {
			animations.resize(scene->mNumAnimations);

			for (auto i = 0u; i < scene->mNumMaterials; i++) {
				animations[i].load(scene->mAnimations[i]);
			}
		}

	} catch (OtException& e) {
		OtLogWarning("Invalid model [{}]: {}", path, e.getShortErrorMessage());
		return State::invalid;
	}

	// output diagnostics
	nodes.debug();

	return State::ready;
}
