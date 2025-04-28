//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <atomic>
#include <cmath>
#include <cstdint>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"

#include "OtGpu.h"
#include "OtUi.h"

#include "OtModel.h"


//
//	OtModel::load
//

void OtModel::load(const std::string& p) {
	// assign a model identifier
	path = p;
	static std::atomic<size_t> nextID = 0;
	id = nextID++;

	// clear current model
	clear();

	// load based on file extension
	auto ext = OtPath::getExtension(path);

	if (ext == ".json") {
		loadJSON();

	} else {
		loadModel(path);
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
	runningAnimation = false;
	isTransitioningAnimation = false;
}


//
//	OtModel::startAnimation
//

void OtModel::startAnimation(const std::string& name) {
	if (hasAnimation(name)) {
		currentAnimation = animationIndex[name];
		runningAnimation = true;
		currentAnimationTime = 0.0f;
		currentAnimationDuration = animations[currentAnimation].getDuration();

	} else {
		OtLogError("Unknown animation [{}] for model", name);
	}
}


//
//	OtModel::stopAnimation
//

void OtModel::stopAnimation() {
	runningAnimation = false;
	isTransitioningAnimation = false;
	nodes.resetAnimationTransforms();
	nodes.updateModelTransforms();
}


//
//	OtModel::fadeToAnimation
//

void OtModel::fadeToAnimation(const std::string& name, float duration) {
	if (!runningAnimation) {
		startAnimation(name);

	} else if (hasAnimation(name)) {
		auto animation = animationIndex[name];

		if (isTransitioningAnimation) {
			animationStack.emplace_back(animation, duration);

		} else {
			startAnimationFade(animation, duration);
		}

	} else {
		OtLogError("Unknown animation [{}] for model", name);
	}
}


//
//	OtModel::update
//

void OtModel::update() {
	// update animations (if required)
	if (runningAnimation) {
		float dt = ImGui::GetIO().DeltaTime;
		nodes.resetAnimationTransforms();

		if (isTransitioningAnimation) {
			animationTransionTime += dt;
			auto ratio = animationTransionTime / animationTransionDuration;

			if (ratio >= 1.0f) {
				currentAnimation = nextAnimation;
				currentAnimationTime = std::fmod(nextAnimationTime + dt, nextAnimationDuration);
				currentAnimationDuration = nextAnimationDuration;
				animations[currentAnimation].update(currentAnimationTime, nodes, 0);
				nodes.updateAnimationTransforms();

				if (animationStack.empty()) {
					isTransitioningAnimation = false;

				} else {
					auto [animation, seconds] = animationStack.back();
					animationStack.clear();
					startAnimationFade(animation, seconds);
				}

			} else {
				float fadeOutRatio = currentAnimationDuration / nextAnimationDuration;
				float fadeInRatio = nextAnimationDuration / currentAnimationDuration;

				float fadeOutMultiplier = glm::mix(1.0f, fadeOutRatio, ratio);
				float fadeInMultiplier = glm::mix(fadeInRatio, 1.0f, ratio);

				currentAnimationTime += dt * fadeOutMultiplier;
				currentAnimationTime = std::fmod(currentAnimationTime, currentAnimationDuration);

				nextAnimationTime += dt * fadeInMultiplier;
				nextAnimationTime = std::fmod(nextAnimationTime, nextAnimationDuration);

				animations[currentAnimation].update(currentAnimationTime, nodes, 0);
				animations[nextAnimation].update(nextAnimationTime, nodes, 1);
				nodes.updateAnimationTransforms(ratio);
			}

		} else {
			currentAnimationTime = std::fmod(currentAnimationTime + dt, currentAnimationDuration);
			animations[currentAnimation].update(currentAnimationTime, nodes, 0);
			nodes.updateAnimationTransforms();
		}

		nodes.updateModelTransforms();
	}
}


//
//	OtModel::getRenderList
//

std::vector<OtModel::RenderCommand>& OtModel::getRenderList(const glm::mat4& modelTransform) {
	// process all nodes and find meshes
	renderList.clear();
	traverseMeshes(0, modelTransform);
	return renderList;
}


//
//	OtModel::renderDetails
//

void OtModel::renderDetails() {
	if (ImGui::BeginTabBar("Tabs")) {
		if (ImGui::BeginTabItem("Meshes")) {
			renderMeshes();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Materials")) {
			renderMaterials();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Textures")) {
			renderTextures();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Nodes")) {
			renderNodes();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animations")) {
			ImGui::EndTabItem();
			renderAnimations();
		}

		ImGui::EndTabBar();
	}
}


//
//	OtModel::loadJSON
//

void OtModel::loadJSON() {
	// parse json
	std::string text;
	OtText::load(path, text);

	auto basedir = OtPath::getParent(path);
	auto data = nlohmann::json::parse(text);

	if (data.contains("model")) {
		if (!data["model"].is_string()) {
			OtLogError("Invalid model file at [{}]: 'model' path is not a string", path);
		}

		loadModel(OtPath::join(basedir, data["model"]));

	} else {
		OtLogError("Invalid model file at [{}]: 'model' path missing", path);
	}

	if (data.contains("animations")) {
		auto animationPaths = data["animations"];

		if (!animationPaths.is_array()) {
			OtLogError("Invalid model file at [{}]: 'animations' is not an array", path);
		}

		for (auto& animationPath : animationPaths) {
			if (!animationPath.is_string()) {
				OtLogError("Invalid model file at [{}]: 'animation' path is not a string", path);
			}

			loadAnimations(OtPath::join(basedir, animationPath));
		}
	}
}


//
//	OtModel::loadModel
//

void OtModel::loadModel(const std::string& modelPath) {
	// create an asset importer
	Assimp::Importer importer;
	// importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

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
	const aiScene* scene = importer.ReadFile(modelPath, static_cast<unsigned int>(flags));

	// ensure model was loaded correctly
	if (scene == nullptr) {
		OtLogError("Unable to load model [{}], error: {}", modelPath, importer.GetErrorString());
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtLogError("Incomplete model [{}]", modelPath);
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
			animationIndex[animations[i].getName()] = i;
		}
	}
}


//
//	OtModel::loadAnimations
//

void OtModel::loadAnimations(const std::string& animationPath) {
	// create an asset importer
	Assimp::Importer importer;
	// importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	// determine the import flags
	auto flags =
		aiProcess_ValidateDataStructure |
		aiProcess_GlobalScale;

	if (!OtGpuHasOriginBottomLeft()) {
		flags |= aiProcess_FlipUVs;
	}

	// read the model file
	const aiScene* scene = importer.ReadFile(animationPath, static_cast<unsigned int>(flags));

	// ensure model was loaded correctly
	if (scene == nullptr) {
		OtLogError("Unable to load model [{}], error: {}", animationPath, importer.GetErrorString());
	}

	// load the nodes
	OtModelNodes animationNodes;
	animationNodes.load(scene->mRootNode);

	// load the animations
	if (scene->HasAnimations()) {
		auto offset = animations.size();

		for (auto i = 0u; i < scene->mNumAnimations; i++) {
			auto& animation = animations.emplace_back();
			animation.load(scene->mAnimations[i], nodes);
			animationIndex[animation.getName()] = offset + i;
		}

	} else {
		OtLogError("File [{}] has no animations", animationPath);
	}
}


//
//	OtModel::startAnimationFade
//

void OtModel::startAnimationFade(size_t animation, float duration) {
	nextAnimation = animation;
	nextAnimationDuration = animations[animation].getDuration();
	nextAnimationTime = currentAnimationTime / currentAnimationDuration * nextAnimationDuration;
	animationTransionTime = 0.0f;
	animationTransionDuration = duration;
	isTransitioningAnimation = true;
}


//
//	OtModel::traverseMeshes
//

void OtModel::traverseMeshes(size_t nodeID, const glm::mat4& modelTransform) {
	auto& node = nodes.getNode(nodeID);

	for (auto meshID : node.meshes) {
		auto& mesh = meshes.at(meshID);
		auto bones = mesh.getBoneCount();
		auto& cmd = renderList.emplace_back();
		cmd.mesh = &mesh;
		cmd.material = materials[mesh.getMaterialIndex()].getMaterial();
		cmd.animation = animations.size() && bones > 0;

		if (cmd.animation) {
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


//
//	OtModel::renderMeshes
//

void OtModel::renderMeshes() {
	if (ImGui::BeginTable("Meshes", 6, tableFlags)) {
		ImGui::TableSetupColumn("Name", columnFlags);
		ImGui::TableSetupColumn("Vertices", columnFlags);
		ImGui::TableSetupColumn("Indices", columnFlags);
		ImGui::TableSetupColumn("Material", columnFlags);
		ImGui::TableSetupColumn("Bones", columnFlags);
		ImGui::TableSetupColumn("Root", columnFlags);
		ImGui::TableHeadersRow();

		for (auto& mesh : meshes) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(mesh.name.c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(mesh.vertices.size()).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(mesh.indices.size()).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(mesh.getMaterialIndex()).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(mesh.bones.size()).c_str());

			if (mesh.hasRootNode()) {
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(std::to_string(mesh.getRootNode()).c_str());
			}
		}

		ImGui::EndTable();
	}
}


//
//	OtModel::renderMaterials
//

void OtModel::renderMaterials() {
	if (ImGui::BeginTable("Materials", 2, tableFlags)) {
		ImGui::TableSetupColumn("ID", columnFlags);
		ImGui::TableSetupColumn("Name", columnFlags);
		ImGui::TableHeadersRow();

		for (size_t i = 0; i < materials.size(); i++) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(i).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(materials[i].name.c_str());
		}

		ImGui::EndTable();
	}
}


//
//	OtModel::renderTextures
//

void OtModel::renderTextures() {
	if (ImGui::BeginTable("Textures", 2, tableFlags)) {
		ImGui::TableSetupColumn("ID", columnFlags);
		ImGui::TableSetupColumn("Name", columnFlags);
		ImGui::TableHeadersRow();

		for (size_t i = 0; i < textures.size(); i++) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(i).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(textures[i].asset.getPath().c_str());
		}

		ImGui::EndTable();
	}
}


//
//	OtModel::renderNodes
//

void OtModel::renderNodes() {
	if (ImGui::BeginTable("Nodes", 4, tableFlags)) {
		ImGui::TableSetupColumn("Name", columnFlags);
		ImGui::TableSetupColumn("ID", columnFlags);
		ImGui::TableSetupColumn("Meshes", columnFlags);
		ImGui::TableSetupColumn("Transforms", columnFlags);
		ImGui::TableHeadersRow();

		renderNode(0);
		ImGui::EndTable();
	}
}


//
//	OtModel::renderNode
//

void OtModel::renderNode(size_t nodeID) {
	auto& node = nodes.getNode(nodeID);

	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	bool open = ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
	ImGui::TableNextColumn();
	ImGui::TextUnformatted(std::to_string(node.id).c_str());
	ImGui::TableNextColumn();
	ImGui::TextUnformatted(std::to_string(node.meshes.size()).c_str());
	ImGui::TableNextColumn();
	renderNodeTransforms(nodeID);

	if (open) {
		for (auto& child : node.children) {
			renderNode(child);
		}

		ImGui::TreePop();
	}
}


//
//	OtModel::renderNodeTransforms
//

void OtModel::renderNodeTransforms(size_t nodeID) {
	if (ImGui::Button("View", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("NodeTransformPopup");
	}

	if (ImGui::BeginPopup("NodeTransformPopup")) {
		auto& node = nodes.getNode(nodeID);
		ImGui::PushItemWidth(300.0f);
		OtUi::viewMat4("Local", node.localTransform);
		ImGui::TextUnformatted("");
		OtUi::viewMat4("Model", node.modelTransform);
		ImGui::PopItemWidth();
		ImGui::EndPopup();
	}
}


//
//	OtModel::renderAnimations
//

void OtModel::renderAnimations() {
	if (ImGui::BeginTable("Animations", 4, tableFlags)) {
		ImGui::TableSetupColumn("ID", columnFlags);
		ImGui::TableSetupColumn("Name", columnFlags);
		ImGui::TableSetupColumn("Duration", columnFlags);
		ImGui::TableSetupColumn("Channels", columnFlags);
		ImGui::TableHeadersRow();

		for (size_t i = 0; i < animations.size(); i++) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(i).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(animations[i].name.c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(animations[i].duration).c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(std::to_string(animations[i].channels.size()).c_str());
		}

		ImGui::EndTable();
	}
}
