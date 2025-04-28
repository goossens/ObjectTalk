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
#include <unordered_map>
#include <utility>
#include <vector>

#include "OtAABB.h"

#include "OtModelAnimation.h"
#include "OtModelMaterial.h"
#include "OtModelMesh.h"
#include "OtModelNodes.h"
#include "OtModelTexture.h"

//
//	OtModel
//

class OtModel {
public:
	// load the model
	void load(const std::string& path);

	// clear the model
	void clear();

	// access animations
	inline bool hasAnimation(const std::string& name) { return animationIndex.find(name) != animationIndex.end(); }
	inline size_t getAnimationCount() { return animations.size(); }
	inline std::string getAnimationName(size_t index) { return animations[index].getName(); }

	// change animations
	void startAnimation(const std::string& name);
	void stopAnimation();
	void fadeToAnimation(const std::string& name, float duration);
	inline bool isAnimating() { return runningAnimation; }

	// get bounding box
	inline OtAABB& getAABB() { return aabb; }

	// update the model for the current frame
	void update();

	// render list access
	class RenderCommand {
	public:
		OtModelMesh* mesh;
		std::shared_ptr<OtMaterial> material;
		bool animation;
		std::vector<glm::mat4> transforms;
	};

	std::vector<RenderCommand>& getRenderList(const glm::mat4& modelTransform);

	// render debugging information
	void renderDetails();

private:
	// mode elements
	OtModelNodes nodes;
	std::vector<OtModelMesh> meshes;
	std::vector<OtModelMaterial> materials;
	std::vector<OtModelTexture> textures;
	std::vector<OtModelAnimation> animations;
	std::unordered_map<std::string, size_t> animationIndex;
	std::vector<std::pair<size_t, float>> animationStack;

	// model identifier
	std::string path;
	size_t id;

	// animation support
	bool runningAnimation = false;

	size_t currentAnimation;
	float currentAnimationTime;
	float currentAnimationDuration;

	size_t nextAnimation;
	float nextAnimationTime;
	float nextAnimationDuration;

	bool isTransitioningAnimation = false;
	float animationTransionTime;
	float animationTransionDuration;

	// rendering support
	OtAABB aabb;
	std::vector<RenderCommand> renderList;

	// support functions
	void loadJSON();
	void loadModel(const std::string& path);
	void loadAnimations(const std::string& path);
	void startAnimationFade(size_t animation, float duration);
	void traverseMeshes(size_t nodeID, const glm::mat4& modelTransform);

	// render model details
	void renderMeshes();
	void renderMaterials();
	void renderTextures();
	void renderNodes();
	void renderNode(size_t nodeID);
	void renderNodeTransforms(size_t nodeID);
	void renderAnimations();

	// rendering flags
	static constexpr ImGuiTableFlags tableFlags =
		ImGuiTableFlags_BordersV |
		ImGuiTableFlags_BordersOuterH |
		ImGuiTableFlags_RowBg;

	static constexpr ImGuiTableColumnFlags columnFlags =
		ImGuiTableColumnFlags_NoHide;
};
