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
#include "imgui.h"

#include "OtLog.h"

#include "OtGpu.h"

#include "OtModel.h"


//
//	OtModel::load
//

void OtModel::load(const std::string& path) {
	// clear current model
	clear();

	// assign a model identifier
	static std::atomic<size_t> nextID = 0;
	id = nextID++;

	// create an asset importer
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	// determine the import flags
	auto flags =
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |
		aiProcess_GenNormals |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes |
		aiProcess_GenBoundingBoxes |
		aiProcess_ValidateDataStructure |
		aiProcess_GlobalScale;

	if (!OtGpuHasOriginBottomLeft()) {
		flags |= aiProcess_FlipUVs;
	}

	// read the model file
	const aiScene* scene = importer.ReadFile(path, static_cast<unsigned int>(flags));

	// ensure model was loaded correctly
	if (scene == nullptr) {
		OtLogError("Unable to load model [{}], error: {}", path, importer.GetErrorString());
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtLogError("Incomplete model [{}]", path);
	}

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

		for (auto i = 0u; i < scene->mNumAnimations; i++) {
			animations[i].load(scene->mAnimations[i], nodes);
		}
	}
}


//
//	OtModel::clear
//

void OtModel::clear() {
	aabb.clear();
	nodes.clear();
	meshes.clear();
	materials.clear();
	textures.clear();
	animations.clear();
	resetAnimation();
}


//
//	OtModel::resetAnimation
//

void OtModel::resetAnimation() {
	currentAnimation = 0;
}


//
//	OtModel::setAnimation
//

void OtModel::setAnimation(size_t animation) {
	if (animation < animations.size()) {
		isTransitioningAnimation = false;
		currentAnimation = animation;
	}
}


//
//	OtModel::fadeToAnimation
//

void OtModel::fadeToAnimation(size_t animation, float seconds) {
	if (animation < animations.size()) {
		if (isTransitioningAnimation) {
			currentAnimation = nextAnimation;
		}

		nextAnimation = animation;
		isTransitioningAnimation = true;
		animationTransionTime = seconds;
		animationRatio = 0.0f;
	}
}


//
//	OtModel::getRenderList
//

std::vector<OtModel::RenderCommand>& OtModel::getRenderList(const glm::mat4& modelTransform) {
	// update animations (if required)
	if (animations.size()) {
		auto time = static_cast<float>(ImGui::GetTime());
		nodes.resetAnimationTransforms();

		if (isTransitioningAnimation) {
			animations[currentAnimation].update(time, nodes, 0);
			animations[nextAnimation].update(time, nodes, 1);
			nodes.updateAnimationTransforms(animationRatio);

		} else {
			animations[currentAnimation].update(time, nodes, 0);
			nodes.updateAnimationTransforms();
		}

		nodes.updateModelTransforms();
	}

	// process all nodes and find meshes
	renderList.clear();
	traverseMeshes(0, modelTransform);
	return renderList;
}


//
//	OtModel::traverseMeshes
//

void OtModel::traverseMeshes(size_t nodeID, const glm::mat4& modelTransform) {
	auto& node = nodes.getNode(nodeID);

	for (auto meshID : node.meshes) {
		auto& mesh = meshes.at(meshID);
		auto& cmd = renderList.emplace_back();
		cmd.mesh = &mesh;
		cmd.material = materials[mesh.getMaterialIndex()].getMaterial();
		cmd.animation = animations.size() && mesh.getBoneCount();

		if (cmd.animation) {
			auto bones = mesh.getBoneCount();
			cmd.transforms.reserve(bones);

			for (size_t i = 0; i < bones; i++) {
				auto& bone = mesh.getBone(i);
				cmd.transforms.emplace_back(modelTransform * nodes.getNode(bone.node).modelTransform * bone.offsetTransform);
			}

		} else {
			cmd.transforms.emplace_back(modelTransform * node.modelTransform);
		}
	}

	for (auto child : node.children) {
		traverseMeshes(child, modelTransform);
	}
}
