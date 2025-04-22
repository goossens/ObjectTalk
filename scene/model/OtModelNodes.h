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
#include <unordered_map>

#include "assimp/scene.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"


//
//	OtModelNodes
//

class OtModelNodes {
public:
	// load the nodes into a hierarchy
	void load(const aiNode* rootnode);

	// clar the nodes structure
	void clear();

	// update transforms
	void resetAnimationTransforms();
	void setAnimationTransformParts(size_t nodeID, size_t slot, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
	void updateAnimationTransforms(float ratio=1.0f);
	void updateModelTransforms(size_t nodeID, const glm::mat4& transform);
	void updateModelTransforms();

	// properties
	class Node {
	public:
		size_t id;
		std::string name;
		glm::mat4 localTransform;
		glm::mat4 animationTransform;
		glm::mat4 modelTransform;
		std::vector<size_t> children;
		std::vector<size_t> meshes;

		struct TransformParts {
			bool available;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};

		TransformParts transformParts[2];
	};

	// access nodes
	inline bool hasNode(const std::string& name) { return index.find(name) != index.end(); }
	inline Node& getNode(const std::string& name) { return nodes[index[name]]; }
	inline Node& getNode(size_t nodeID) { return nodes[nodeID]; }
	inline size_t getNodeID(const std::string& name) { return index[name]; }

private:
	// properties
	std::vector<Node> nodes;
	std::unordered_map<std::string, size_t> index;

	size_t addNode(const aiNode* node);
};
