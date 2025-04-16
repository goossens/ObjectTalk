//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <limits>

#include "OtLog.h"

#include "OtModelNodes.h"
#include "OtModelUtils.h"


//
//	OtModelNodes::load
//

void OtModelNodes::load(const aiNode* node) {
	// get all the nodes
	nodes.clear();
	addNode(node);

	// calculate the inverse transformation matrix for root node
	auto matrix = toMat4(node->mTransformation);
	globalInverseTransform = glm::inverse(matrix);

	// mark all parents
	markParent(0, std::numeric_limits<size_t>::max());
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

	for (auto i = 0u; i < node->mNumChildren; i++) {
		auto child = addNode(node->mChildren[i]);
		nodes[id].children.emplace_back(child);
	}

	return id;
}


//
//	OtModelNodes::markParent
//

void OtModelNodes::markParent(size_t id, size_t parent) {
	auto& node = nodes.at(id);
	node.parent = parent;

	for (auto child : node.children) {
		markParent(child, node.id);
	}
}


//
//	OtModelNodes::printDebug
//

void OtModelNodes::printDebug(size_t id, size_t indent) {
	auto& node = nodes.at(id);
	OtLogDebug("{}Node {}: {}", std::string(indent * 4, ' '), id, node.name);

	for (auto child : node.children) {
		printDebug(child, indent + 1);
	}
}
