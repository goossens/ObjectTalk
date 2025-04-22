//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtModelNodes.h"
#include "OtModelUtils.h"


//
//	OtModelNodes::load
//

void OtModelNodes::load(const aiNode* rootnode) {
	// get all nodes and calculate matrices
	addNode(rootnode);
	resetAnimationTransforms();
	updateModelTransforms();
}


//
//	OtModelNodes::addNode
//

size_t OtModelNodes::addNode(const aiNode* node) {
	auto id = nodes.size();
	auto name = std::string(node->mName.C_Str());
	index[name] = id;

	nodes.emplace_back();
	nodes[id].id = id;
	nodes[id].name = name;
	nodes[id].localTransform = toMat4(node->mTransformation);

	for (auto i = 0u; i < node->mNumMeshes; i++) {
		nodes[id].meshes.emplace_back(node->mMeshes[i]);
	}

	for (auto i = 0u; i < node->mNumChildren; i++) {
		auto child = addNode(node->mChildren[i]);
		nodes[id].children.emplace_back(child);
	}

	return id;
}


//
//	OtModelNodes::clear
//

void OtModelNodes::clear() {
	nodes.clear();
	index.clear();
}


//
//	OtModelNodes::resetAnimationTransforms
//

void OtModelNodes::resetAnimationTransforms() {
	for (auto& node : nodes) {
		node.animationTransform = node.localTransform;
		node.transformParts[0].available = false;
		node.transformParts[1].available = false;
	}
}


//
//	OtModelNodes::setAnimationTransformParts
//

void OtModelNodes::setAnimationTransformParts(size_t nodeID, size_t slot, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
	auto& transformParts = nodes.at(nodeID).transformParts[slot];
	transformParts.available = true;
	transformParts.position = position;
	transformParts.rotation = rotation;
	transformParts.scale = scale;
}


//
//	OtModelNodes::updateAnimationTransforms
//

void OtModelNodes::updateAnimationTransforms(float ratio) {
	for (auto& node : nodes) {
		if (node.transformParts[0].available && node.transformParts[1].available) {
			auto position = glm::mix(node.transformParts[0].position, node.transformParts[1].position, ratio);
			auto rotation = glm::slerp(node.transformParts[0].rotation, node.transformParts[1].rotation, ratio);
			auto scale = glm::mix(node.transformParts[0].scale, node.transformParts[1].scale, ratio);

			node.animationTransform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::scale(glm::toMat4(rotation), scale);

		} else if (node.transformParts[0].available) {
			node.animationTransform =
				glm::translate(glm::mat4(1.0f), node.transformParts[0].position) *
				glm::scale(glm::toMat4(node.transformParts[0].rotation), node.transformParts[0].scale);

		} else if (node.transformParts[1].available) {
			node.animationTransform =
				glm::translate(glm::mat4(1.0f), node.transformParts[1].position) *
				glm::scale(glm::toMat4(node.transformParts[1].rotation), node.transformParts[1].scale);
		}
	}
}


//
//	OtModelNodes::updateModelTransforms
//

void OtModelNodes::updateModelTransforms(size_t nodeID, const glm::mat4& parentTransform) {
	auto& node = nodes.at(nodeID);
	node.modelTransform = nodeID == 0 ? node.animationTransform : parentTransform * node.animationTransform;

	for (auto child : node.children) {
		updateModelTransforms(child, node.modelTransform);
	}
}


//
//	OtModelNodes::updateModelTransforms
//

void OtModelNodes::updateModelTransforms() {
	if (nodes.size()) {
		glm::mat4 identity(1.0f);
		updateModelTransforms(0, identity);
	}
}
