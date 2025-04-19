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

	// change animations
	void resetAnimation();
	void setAnimation(size_t animation);
	void fadeToAnimation(size_t animation, float seconds);

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

	// model identifier
	size_t id;

	// animation support
	bool isTransitioningAnimation;
	size_t currentAnimation;
	size_t nextAnimation;
	float animationTransionTime;
	float animationRatio;

	// rendering support
	OtAABB aabb;
	std::vector<RenderCommand> renderList;

	// support functions
	void traverseMeshes(size_t nodeID, const glm::mat4& modelTransform);
};
