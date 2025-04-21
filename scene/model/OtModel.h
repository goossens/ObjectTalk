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
	inline size_t getAnimationIndex(const std::string& name) { return animationIndex[name]; }

	// change animations
	void resetAnimation();
	void setAnimation(size_t animation);
	void fadeToAnimation(size_t animation, float seconds);
	inline void setAnimation(const std::string& name) { setAnimation(getAnimationIndex(name)); }
	inline void fadeToAnimation(const std::string& name, float seconds) { fadeToAnimation(getAnimationIndex(name), seconds); }

	// get bounding box
	inline OtAABB& getAABB() { return aabb; }

	// render list access
	class RenderCommand {
	public:
		OtModelMesh* mesh;
		std::shared_ptr<OtMaterial> material;
		bool animation;
		std::vector<glm::mat4> transforms;
	};

	std::vector<RenderCommand>& getRenderList(const glm::mat4& modelTransform);

private:
	// our nodes, meshes, materials, textures and animations
	OtModelNodes nodes;
	std::vector<OtModelMesh> meshes;
	std::vector<OtModelMaterial> materials;
	std::vector<OtModelTexture> textures;
	std::vector<OtModelAnimation> animations;
	std::unordered_map<std::string, size_t> animationIndex;

	// model identifier
	size_t id;

	// animation support
	bool isTransitioningAnimation;
	size_t currentAnimation;
	size_t nextAnimation;
	float animationTransionStart;
	float animationTransionDuration;

	// rendering support
	OtAABB aabb;
	std::vector<RenderCommand> renderList;

	// support functions
	void traverseMeshes(size_t nodeID, const glm::mat4& modelTransform);
};
